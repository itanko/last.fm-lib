dirs := . inc src
wcards:= $(addsuffix /*.c, $(dirs))

lastfm: $(notdir $(patsubst %.c, %.o, $(wildcard $(wcards))))
	ar rs $(addsuffix .a, $(addprefix lib, $@)) $^ 

VPATH := $(dirs)

%.o: %.c
	gcc -pedantic -c $< $(addprefix -I, $(dirs)) 

clean:
	rm *.o  *.a
