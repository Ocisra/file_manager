let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/file_manager
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
set shortmess=aoO
badd +1 ~/ft-detect/test/main.cpp
badd +1 session.vim
badd +1 main.cpp
badd +97 miller.cpp
badd +42 miller.hpp
badd +149 path.cpp
badd +55 path.hpp
badd +1 log.hpp
badd +1 log.cpp
badd +1 makefile
badd +1 compile_flags.txt
badd +1 TODO
badd +1 README.md
badd +1 .gitignore
badd +12 config.cpp
badd +6 config.hpp
argglobal
%argdel
$argadd file_manager/
set stal=2
tabnew
tabnew
tabrewind
edit main.cpp
set splitbelow splitright
wincmd _ | wincmd |
split
1wincmd k
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd w
wincmd t
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 45 + 47) / 94)
exe 'vert 1resize ' . ((&columns * 59 + 59) / 119)
exe '2resize ' . ((&lines * 45 + 47) / 94)
exe 'vert 2resize ' . ((&columns * 59 + 59) / 119)
exe '3resize ' . ((&lines * 45 + 47) / 94)
argglobal
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
81
normal! zo
let s:l = 80 - ((39 * winheight(0) + 22) / 45)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
80
normal! 0
wincmd w
argglobal
if bufexists("config.hpp") | buffer config.hpp | else | edit config.hpp | endif
if &buftype ==# 'terminal'
  silent file config.hpp
endif
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 13 - ((12 * winheight(0) + 22) / 45)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
13
normal! 042|
wincmd w
argglobal
if bufexists("config.cpp") | buffer config.cpp | else | edit config.cpp | endif
if &buftype ==# 'terminal'
  silent file config.cpp
endif
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 1 - ((0 * winheight(0) + 22) / 45)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
1
normal! 021|
wincmd w
3wincmd w
exe '1resize ' . ((&lines * 45 + 47) / 94)
exe 'vert 1resize ' . ((&columns * 59 + 59) / 119)
exe '2resize ' . ((&lines * 45 + 47) / 94)
exe 'vert 2resize ' . ((&columns * 59 + 59) / 119)
exe '3resize ' . ((&lines * 45 + 47) / 94)
tabnext
edit log.hpp
set splitbelow splitright
wincmd _ | wincmd |
split
wincmd _ | wincmd |
split
wincmd _ | wincmd |
split
3wincmd k
wincmd w
wincmd w
wincmd w
wincmd t
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 22 + 47) / 94)
exe '2resize ' . ((&lines * 22 + 47) / 94)
exe '3resize ' . ((&lines * 22 + 47) / 94)
exe '4resize ' . ((&lines * 22 + 47) / 94)
argglobal
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 30 - ((7 * winheight(0) + 11) / 22)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
30
normal! 0
wincmd w
argglobal
if bufexists("log.cpp") | buffer log.cpp | else | edit log.cpp | endif
if &buftype ==# 'terminal'
  silent file log.cpp
endif
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 12 - ((4 * winheight(0) + 11) / 22)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
12
normal! 027|
wincmd w
argglobal
if bufexists("config.hpp") | buffer config.hpp | else | edit config.hpp | endif
if &buftype ==# 'terminal'
  silent file config.hpp
endif
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 16 - ((11 * winheight(0) + 11) / 22)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
16
normal! 011|
wincmd w
argglobal
if bufexists("config.cpp") | buffer config.cpp | else | edit config.cpp | endif
if &buftype ==# 'terminal'
  silent file config.cpp
endif
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 24 - ((13 * winheight(0) + 11) / 22)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
24
normal! 09|
wincmd w
exe '1resize ' . ((&lines * 22 + 47) / 94)
exe '2resize ' . ((&lines * 22 + 47) / 94)
exe '3resize ' . ((&lines * 22 + 47) / 94)
exe '4resize ' . ((&lines * 22 + 47) / 94)
tabnext
edit makefile
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd t
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 48 + 47) / 94)
exe 'vert 1resize ' . ((&columns * 59 + 59) / 119)
exe '2resize ' . ((&lines * 42 + 47) / 94)
exe 'vert 2resize ' . ((&columns * 59 + 59) / 119)
exe '3resize ' . ((&lines * 48 + 47) / 94)
exe 'vert 3resize ' . ((&columns * 59 + 59) / 119)
exe '4resize ' . ((&lines * 42 + 47) / 94)
exe 'vert 4resize ' . ((&columns * 59 + 59) / 119)
argglobal
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 22 - ((21 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
22
normal! 030|
wincmd w
argglobal
if bufexists("README.md") | buffer README.md | else | edit README.md | endif
if &buftype ==# 'terminal'
  silent file README.md
endif
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 3 - ((2 * winheight(0) + 21) / 42)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
3
normal! 017|
wincmd w
argglobal
if bufexists("compile_flags.txt") | buffer compile_flags.txt | else | edit compile_flags.txt | endif
if &buftype ==# 'terminal'
  silent file compile_flags.txt
endif
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 3 - ((2 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
3
normal! 02|
wincmd w
argglobal
if bufexists("TODO") | buffer TODO | else | edit TODO | endif
if &buftype ==# 'terminal'
  silent file TODO
endif
setlocal fdm=syntax
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=5
setlocal fml=1
setlocal fdn=10
setlocal fen
let s:l = 3 - ((2 * winheight(0) + 21) / 42)
if s:l < 1 | let s:l = 1 | endif
exe s:l
normal! zt
3
normal! 0
wincmd w
exe '1resize ' . ((&lines * 48 + 47) / 94)
exe 'vert 1resize ' . ((&columns * 59 + 59) / 119)
exe '2resize ' . ((&lines * 42 + 47) / 94)
exe 'vert 2resize ' . ((&columns * 59 + 59) / 119)
exe '3resize ' . ((&lines * 48 + 47) / 94)
exe 'vert 3resize ' . ((&columns * 59 + 59) / 119)
exe '4resize ' . ((&lines * 42 + 47) / 94)
exe 'vert 4resize ' . ((&columns * 59 + 59) / 119)
tabnext 1
set stal=1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0&& getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20 winminheight=1 winminwidth=1 shortmess=filnxtToOFc
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
