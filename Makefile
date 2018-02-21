ringmaster :ringmaster.c player.c potato.h
	gcc -o ringmaster -pedantic -std=gnu99 -Wall -Werror potato.h ringmaster.c 
	gcc -o player -pedantic -std=gnu99 -Wall -Werror potato.h player.c 
