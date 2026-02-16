
/***************************************************************************

    Graphics Subsystem Processor (GSP)

    Notes:
        GSP is the graphics subsystem processor, a custom TMS34010-based board.  It is
		responsible for rendering the video for the RTP.

***************************************************************************/

#include "emu.h"
#include "cpu/tms34010/tms34010.h"
#include "video/tlc34076.h"
#include "screen.h"

namespace {

/*************************************
 *
 *  Structs
 *
 *************************************/

class gsp_state : public driver_device
{
public:
	gsp_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_tlc34076(*this, "tlc34076"),
		m_tms_vram(*this, "tms_vram"),

		m_tms(*this, "tms")
	{
	}

	void gsp(machine_config &config);

private:
	required_device<tlc34076_device> m_tlc34076;
	required_shared_ptr<uint16_t> m_tms_vram;
	uint8_t m_porta_latch = 0;
	uint8_t m_ay_sel = 0;
	uint8_t m_lastdataw = 0;
	uint16_t m_lastdatar = 0;
	uint16_t ramdac_r(offs_t offset);
	void ramdac_w(offs_t offset, uint16_t data);
	void tms_w(offs_t offset, uint8_t data);
	uint8_t tms_r(offs_t offset);
	void tms_irq(int state);
	TMS340X0_SCANLINE_RGB32_CB_MEMBER(scanline_update);
	virtual void machine_reset() override ATTR_COLD;
	virtual void video_start() override ATTR_COLD;
	required_device<tms34010_device> m_tms;
	void tms_program_map(address_map &map) ATTR_COLD;
};


/*************************************
 *
 *  Initialisation
 *
 *************************************/

void gsp_state::machine_reset()
{
}


/*************************************
 *
 *  Video Functions
 *
 *************************************/

void gsp_state::video_start()
{
}

TMS340X0_SCANLINE_RGB32_CB_MEMBER(gsp_state::scanline_update)
{
	pen_t const *const pens = m_tlc34076->pens();
	uint16_t *vram = &m_tms_vram[(params->rowaddr << 8) & 0x3ff00];
	uint32_t *const dest = &bitmap.pix(scanline);
	int coladdr = params->coladdr;

	for (int x = params->heblnk; x < params->hsblnk; x += 2)
	{
		uint16_t pixels = vram[coladdr++ & 0xff];
		dest[x + 0] = pens[pixels & 0xff];
		dest[x + 1] = pens[pixels >> 8];
	}
}

uint16_t gsp_state::ramdac_r(offs_t offset)
{
	offset = (offset >> 12) & ~4;

	if (offset & 8)
		offset = (offset & ~8) | 4;

	return m_tlc34076->read(offset);
}

void gsp_state::ramdac_w(offs_t offset, uint16_t data)
{
	offset = (offset >> 12) & ~4;

	if (offset & 8)
		offset = (offset & ~8) | 4;

	m_tlc34076->write(offset, data);
}


/*************************************
 *
 *  CPU Communications
 *
 *************************************/

void gsp_state::tms_irq(int state)
{
}

/*************************************
 *
 *  Video CPU memory handlers
 *
 *************************************/

void gsp_state::tms_program_map(address_map &map)
{
	map(0x00000000, 0x003fffff).ram().share("tms_vram");
	map(0x00440000, 0x004fffff).rw(FUNC(gsp_state::ramdac_r), FUNC(gsp_state::ramdac_w));
	map(0xff800000, 0xffbfffff).rom().mirror(0x00400000).region("tms", 0);
}


/*************************************
 *
 *  Input definitions
 *
 *************************************/

static INPUT_PORTS_START( gsp )
INPUT_PORTS_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

void gsp_state::gsp(machine_config &config)
{

	TMS34010(config, m_tms, 48000000);
	m_tms->set_addrmap(AS_PROGRAM, &gsp_state::tms_program_map);
	m_tms->set_halt_on_reset(true);
	m_tms->set_pixel_clock(48000000 / 8);
	m_tms->set_pixels_per_clock(1);
	m_tms->set_scanline_rgb32_callback(FUNC(gsp_state::scanline_update));
	m_tms->output_int().set(FUNC(gsp_state::tms_irq));

	TLC34076(config, m_tlc34076, 0);
	m_tlc34076->set_bits(tlc34076_device::TLC34076_6_BIT);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_raw(48000000 / 8, 156*4, 0, 100*4, 328, 0, 300); // FIXME
	screen.set_screen_update("tms", FUNC(tms34010_device::tms340x0_rgb32));

}


/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( gsp )
ROM_END


} // anonymous namespace


/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1991, gsp, 0, gsp, gsp, gsp_state, empty_init, ROT0, "Kelly", "GSP",           MACHINE_NOT_WORKING )
