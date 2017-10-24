/* Headers to ft2_* functions ... */
int ft2_init(void);
void ft2_term(void);
Fontheader *ft2_load_font(Virtual * vwk, const char *filename);
long ft2_char_width(Virtual * vwk, Fontheader * font, long ch);
long ft2_text_width(Virtual * vwk, Fontheader * font, short *s, long slen);
Fontheader *ft2_vst_point(Virtual * vwk, long ptsize, short *sizes);
long ft2_text_render_default(Virtual * vwk, unsigned long coords, short *s, long slen);
long ft2_set_effects(Virtual * vwk, Fontheader * font, long effects);
void *ft2_char_bitmap(Virtual * vwk, Fontheader * font, long ch, short *bitmap_info);
void *ft2_char_advance(Virtual * vwk, Fontheader * font, long ch, short *advance_info);
void ft2_xfntinfo(Virtual * vwk, Fontheader * font, long flags, XFNT_INFO * info);
void ft2_fontheader(Virtual * vwk, Fontheader * font, VQT_FHDR * fhdr);
