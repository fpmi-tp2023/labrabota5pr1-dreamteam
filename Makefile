bin/program: obj/main.o obj/server.o obj/server_admin.o
	gcc -o bin/program obj/main.o obj/server.o obj/server_admin.o -lsqlite3

obj/main.o: src/main.c
	gcc -c -o obj/main.o src/main.c

obj/server.o: src/server.c
	gcc -c -o obj/server.o src/server.c

obj/server_admin.o: src/server_admin.c
	gcc -c -o obj/server_admin.o src/server_admin.c

clean:
	rm -rf obj/*.o bin/program
