SECTION rodata_user
PUBLIC _glyphdata, _textdata, _border_h, _border_v, _border_tl, _border_tr, _border_bl, _border_br

_glyphdata:
BINARY  "data/glyph.dat"

_textdata:
BINARY  "data/text.dat"

_border_h:
DB $00,$00,$00,$ff,$ff,$00,$00,$00
_border_v:
DB $18,$18,$18,$18,$18,$18,$18,$18
_border_tl:
DB $00,$00,$00,$1f,$1f,$18,$18,$18
_border_tr:
DB $00,$00,$00,$f8,$f8,$18,$18,$18
_border_bl:
DB $18,$18,$18,$1f,$1f,$00,$00,$00
_border_br:
DB $18,$18,$18,$f8,$f8,$00,$00,$00
