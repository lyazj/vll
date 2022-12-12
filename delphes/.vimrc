func! Compile_Run_CPP()
  exec "w"
  exec "!clear; cd \"%:h\" && root -q -l \"%:t\""
endfunc
autocmd Filetype cpp imap <F5> <ESC>:call Compile_Run_CPP()<CR>
autocmd Filetype cpp nmap <F5>      :call Compile_Run_CPP()<CR>
autocmd Filetype cpp vmap <F5> <ESC>:call Compile_Run_CPP()<CR>

func! Compile_Debug_CPP()
  exec "w"
  exec "!clear; cd \"%:h\""
     \ " && g++ -g -Wall -Wextra -Wshadow \"%:t\" -o \"%:t:r\""
     \ " `root-config --cflags --libs`"
     \ " && (if [ \"%:e\" = \"h\" ]; then"
     \ " rm \"%:t:r\"; else cgdb -q \"%:t:r\"; fi)"
endfunc
autocmd Filetype cpp imap <C-F5> <ESC>:call Compile_Debug_CPP()<CR>
autocmd Filetype cpp nmap <C-F5>      :call Compile_Debug_CPP()<CR>
autocmd Filetype cpp vmap <C-F5> <ESC>:call Compile_Debug_CPP()<CR>
