// license:BSD-3-Clause
// copyright-holders:Raphael Nabet
/* register names for apexc_get_reg & apexc_set_reg */
#ifndef MAME_CPU_APEXC_APEXC_H
#define MAME_CPU_APEXC_APEXC_H

#pragma once

#define MCFG_APEXC_TAPE_READ_CB(_devcb) \
	devcb = &downcast<apexc_cpu_device &>(*device).set_tape_read_cb(DEVCB_##_devcb);

#define MCFG_APEXC_TAPE_PUNCH_CB(_devcb) \
	devcb = &downcast<apexc_cpu_device &>(*device).set_tape_punch_cb(DEVCB_##_devcb);

enum
{
	APEXC_CR =1,    /* control register */
	APEXC_A,        /* accumulator */
	APEXC_R,        /* register */
	APEXC_ML,       /* memory location */
	APEXC_WS,       /* working store */
	APEXC_STATE,    /* whether CPU is running */
};

class apexc_cpu_device : public cpu_device
{
public:
	// construction/destruction
	apexc_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration
	template<class Object> devcb_base &set_tape_read_cb(Object &&object) { return m_tape_read_cb.set_callback(std::forward<Object>(object)); }
	template<class Object> devcb_base &set_tape_punch_cb(Object &&object) { return m_tape_punch_cb.set_callback(std::forward<Object>(object)); }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual uint32_t execute_min_cycles() const override { return 2; }
	virtual uint32_t execute_max_cycles() const override { return 75; }
	virtual uint32_t execute_input_lines() const override { return 0; }
	virtual void execute_run() override;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;

	// device_state_interface overrides
	virtual void state_import(const device_state_entry &entry) override;

	// device_disasm_interface overrides
	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;

	inline uint32_t apexc_readmem(uint32_t address) { return m_program->read_dword((address)<<2); }
	inline void apexc_writemem(uint32_t address, uint32_t data) { m_program->write_dword((address)<<2, (data)); }
	inline void apexc_writemem_masked(uint32_t address, uint32_t data, uint32_t mask) { apexc_writemem((address), (apexc_readmem(address) & ~(mask)) | ((data) & (mask))); }

	uint32_t effective_address(uint32_t address);
	uint32_t word_read(uint32_t address, uint32_t special);
	void word_write(uint32_t address, uint32_t data, uint32_t mask);
	uint8_t papertape_read();
	void papertape_punch(uint8_t data);

	uint32_t load_ml(uint32_t address, uint32_t vector);
	void execute();

	address_space_config m_program_config;

	devcb_read8 m_tape_read_cb;
	devcb_write8 m_tape_punch_cb;

	uint32_t m_a;   /* accumulator */
	uint32_t m_r;   /* register */
	uint32_t m_cr;  /* control register (i.e. instruction register) */
	int m_ml;     /* memory location (current track in working store, and requested word position within track) (10 bits) */
	int m_working_store;  /* current working store (group of 16 tracks) (1-15) */
	int m_current_word;   /* current word position within track (0-31) */

	int m_running;    /* 1 flag: */
						/* running: flag implied by the existence of the stop instruction */
	uint32_t m_pc;  /* address of next instruction for the disassembler */

	address_space *m_program;
	int m_icount;

	// For state
	uint32_t m_ml_full;
	uint32_t m_genpc;
};


DECLARE_DEVICE_TYPE(APEXC, apexc_cpu_device)

#endif // MAME_CPU_APEXC_APEXC_H
