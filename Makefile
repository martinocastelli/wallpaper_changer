change_wallpaper: main.c
	gcc -o $@ $^ -Wall

clean:
	rm tags 
