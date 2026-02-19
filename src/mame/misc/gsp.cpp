// license:BSD-3-Clause
// copyright-holders:David Haywood
/***************************************************************************



***************************************************************************/

#include "emu.h"
#include "cpu/tms34010/tms34010.h"
#include "screen.h"


namespace {

class gsp_state : public driver_device
{
public:
	gsp_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{ }

	void gsp(machine_config &config);

private:
	virtual void machine_reset() override ATTR_COLD;
	virtual void video_start() override ATTR_COLD;

	TMS340X0_SCANLINE_RGB32_CB_MEMBER(scanline_update);

	void gsp_map(address_map &map) ATTR_COLD;

	required_device<tms34010_device> m_maincpu;
};


void gsp_state::video_start()
{
}

TMS340X0_SCANLINE_RGB32_CB_MEMBER(gsp_state::scanline_update)
{
}


void gsp_state::machine_reset()
{
}


void gsp_state::gsp_map(address_map &map)
{
	map(0x08000000, 0x080bffff).ram();
}


static INPUT_PORTS_START( gsp )
INPUT_PORTS_END


void gsp_state::gsp(machine_config &config)
{
	/* basic machine hardware */
	TMS34010(config, m_maincpu, 40_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &gsp_state::gsp_map);
	m_maincpu->set_halt_on_reset(false);
	m_maincpu->set_pixel_clock(22.1184_MHz_XTAL / 22);
	m_maincpu->set_pixels_per_clock(1);
	m_maincpu->set_scanline_rgb32_callback(FUNC(gsp_state::scanline_update));

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_raw(22.1184_MHz_XTAL / 2, 444, 0, 320, 233, 0, 200);
	screen.set_screen_update("maincpu", FUNC(tms34010_device::tms340x0_rgb32));

}


ROM_START( gsp )
	ROM_REGION16_LE( 0x400000, "user1", 0 ) /* 34020 code */

ROM_END


} // anonymous namespace


GAME( 2026, gsp,    0,        gsp,    gsp, gsp_state, empty_init, ROT0, "NGC.",  "Graphics System Processor", MACHINE_NO_SOUND  )
