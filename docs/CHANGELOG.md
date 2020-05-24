## 0.1.0
- initial commit
- setup project structure
- add dependencies
- add nuklear implementations
- add makefiles
- setup ci
- add config handler
- add logger
- add libretro function pointers
- add core loading
- add core selector

## 0.1.1
- add environment callback
- hookup set variables to piccolo
- hookup set variables to gui
- move members into piccolo struct for convenience
- fix all warnings, add asan, add debug builds (bparker)
- expose full path, block extract

## 0.1.2
- add start core for contentless cores
- add load content for cores that require content
- add video rendering
- add audio playback

## 0.1.3
- replace nuklear with imgui
- rewrite backend in C++
- rewrite frontend in C++
- rewrite settings handler
- implement multi instancing (different cores)
- hookup input processing
- hookup input bitmasks
- hookup loading cores without content
- hookup rendering outside imgui window
- implement file selector
- setup ci using github actions
