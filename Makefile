dirs := . inc src
wcards:= $(addsuffix /*.c, $(dirs))

libs := ws2_32 user32 kernel32

lastfm: $(notdir $(patsubst %.c, %.o, $(wildcard $(wcards))))
	gcc $^ $(addprefix -l,$(libs)) -o $@ $(addprefix -I, $(dirs)) 

VPATH := $(dirs)

%.o: %.c
	gcc -c $< $(addprefix -I, $(dirs)) -D DEBUG

clean:
	rm *.o lastfm.exe
