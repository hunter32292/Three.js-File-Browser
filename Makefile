CFLAGS   = -g -std=gnu99 -Wall

SOURCE   = $(shell ls *.c)
HEADERS  = $(shell ls *.h)
OBJECTS	 = ${SOURCE:.c=.o}
PROGRAM  = chippewa

all:	${PROGRAM}

chippewa: ${OBJECTS} ${HEADERS}
	$(CC) $(CFLAGS) -o $@ ${OBJECTS}

%.o:	%.c ${HEADERS} 
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f ${PROGRAM} ${OBJECTS}

.PHONY:	all clean
