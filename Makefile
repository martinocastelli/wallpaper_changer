change_wallpaper.out: main.c tc/*
	gcc -o $@ main.c tc/tc.c -Wall

clean:
	rm tags 
