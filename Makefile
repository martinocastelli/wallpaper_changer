main.out: main.c tc/*
	gcc -O0 -g -o $@ main.c tc/tc.c -Wall

release:
	gcc -o change_wallpaper.out -O3 -Wall main.c tc/tc.c

clean:
	rm -f tags main.out
