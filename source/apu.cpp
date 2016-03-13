#include <stdio.h>
#include <stdlib.h>

#include "gb_apu/Gb_Apu.h"
#include "gb_apu/Multi_Buffer.h"

#include "platform/common/menu.h"
#include "platform/audio.h"
#include "apu.h"
#include "cpu.h"
#include "gameboy.h"

APU::APU(Gameboy* gameboy) {
    this->gameboy = gameboy;

    this->buffer = new Stereo_Buffer();
    this->buffer->set_sample_rate((long) audioGetSampleRate());
    this->buffer->clock_rate(CYCLES_PER_SECOND);

    this->apu = new Gb_Apu();
    this->apu->set_output(this->buffer->center(), this->buffer->left(), this->buffer->right());
}

APU::~APU() {
    delete this->apu;
    delete this->buffer;
}

void APU::reset() {
    audioClear();

    this->lastSoundCycle = 0;
    this->halfSpeed = false;

    this->apu->reset(this->gameboy->gbMode == MODE_CGB ? Gb_Apu::mode_cgb : Gb_Apu::mode_dmg);
    this->buffer->clear();
}

void APU::loadState(FILE* file, int version) {
    gb_apu_state_t apuState;
    fread(&apuState, 1, sizeof(apuState), file);
    fread(&this->lastSoundCycle, 1, sizeof(this->lastSoundCycle), file);
    fread(&this->halfSpeed, 1, sizeof(this->halfSpeed), file);

    this->apu->load_state(apuState);
    audioClear();
}

void APU::saveState(FILE* file) {
    gb_apu_state_t apuState;
    this->apu->save_state(&apuState);

    fwrite(&apuState, 1, sizeof(apuState), file);
    fwrite(&this->lastSoundCycle, 1, sizeof(this->lastSoundCycle), file);
    fwrite(&this->halfSpeed, 1, sizeof(this->halfSpeed), file);
}

void APU::update() {
    if(this->gameboy->cpu->getCycle() >= this->lastSoundCycle + (CYCLES_PER_FRAME << this->halfSpeed)) {
        u32 cycles = (u32) (this->gameboy->cpu->getCycle() - this->lastSoundCycle) >> this->halfSpeed;

        this->apu->end_frame(cycles);
        this->buffer->end_frame(cycles);

        this->lastSoundCycle = this->gameboy->cpu->getCycle();

        if(soundEnabled) {
            long available = this->buffer->samples_avail();
            u32* buf = (u32*) malloc(available / 2 * sizeof(u32));

            long count = this->buffer->read_samples((s16*) buf, available);
            audioPlay(buf, count / 2);

            free(buf);
        } else {
            this->buffer->clear();
        }
    }

    this->gameboy->cpu->setEventCycle(this->lastSoundCycle + (CYCLES_PER_FRAME << this->halfSpeed));
}

u8 APU::read(u16 addr) {
    return (u8) this->apu->read_register((u32) (this->gameboy->cpu->getCycle() - this->lastSoundCycle) >> this->halfSpeed, addr);
}

void APU::write(u16 addr, u8 val) {
    this->apu->write_register((u32) (this->gameboy->cpu->getCycle() - this->lastSoundCycle) >> this->halfSpeed, addr, val);
}

void APU::setHalfSpeed(bool halfSpeed) {
    if(!this->halfSpeed && halfSpeed) {
        this->lastSoundCycle -= this->gameboy->cpu->getCycle() - this->lastSoundCycle;
    } else if(this->halfSpeed && !halfSpeed) {
        this->lastSoundCycle += (this->gameboy->cpu->getCycle() - this->lastSoundCycle) / 2;
    }

    this->halfSpeed = halfSpeed;
}

void APU::setChannelEnabled(int channel, bool enabled) {
    this->apu->set_osc_enabled(channel, enabled);
}

