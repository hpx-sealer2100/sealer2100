#ifndef _GC9503V_H_
#define _GC9503V_H_

#define GC9503V_FORMAT_RGB888 0x70
#define GC9503V_FORMAT_RGB565 0x50

#define  GC9503V_480X800_HSYNC             ((uint16_t)10)      /* Horizontal synchronization */
#define  GC9503V_480X800_HBP               ((uint16_t)20)     /* Horizontal back porch      */
#define  GC9503V_480X800_HFP               ((uint16_t)20)     /* Horizontal front porch     */

#define  GC9503V_480X800_VSYNC             ((uint16_t)8)      /* Vertical synchronization   */
#define  GC9503V_480X800_VBP               ((uint16_t)24)     /* Vertical back porch        */
#define  GC9503V_480X800_VFP               ((uint16_t)12)     /* Vertical front porch       */
#endif
