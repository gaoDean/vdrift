
ALL:
	rm -rf build/vdrift.app
	scons release=1
	cp -r data build/vdrift.app/Contents/Resources/
	cp -r vdrift-mac/Frameworks build/vdrift.app/Contents/
	cp vdrift-mac/Libraries/libiconv.2.dylib build/vdrift.app/Contents/Frameworks/
	cp vdrift-mac/Libraries/libintl.8.dylib build/vdrift.app/Contents/Frameworks/

dev:
	brew install scons bullet sdl2 sdl2_image
	git clone https://github.com/VDrift/vdrift-mac.git
	mv vdrift-mac/Frameworks/SDL2_image.framework/Headers/SDL_image.h vdrift-mac/Frameworks/SDL2.framework/Headers/
	cp -r vdrift-mac src/

open:
	open build/vdrift.app

install:
	rm ~/Applications/VDrift.app/Contents/MacOS/VDrift
	cp build/vdrift.app/Contents/MacOS/vdrift ~/Applications/vdrift.app/Contents/MacOS/
