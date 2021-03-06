INST_DIR = $(HOME)/bin
BIN  = associate_stations_EAF_ABF

# need to include the math library when the math.h header file is used
#cflags = -O -lm
cflags = -O 
#CFLAGS = $(DBG) $(cflags) -arch i386
#FFLAGS = -O2 -ffixed-line-length-none 
CFLAGS = $(DBG) $(cflags) 
FFLAGS = -O2 -ffixed-line-length-none -m64

FC = gfortran
CC = gcc

DBG = 
FOBJS = associate_stations_eaf_abf.o read_line_fm_csvFile.o delaz.o

$(BIN) : $(FOBJS)
	$(CC) $(CFLAGS) $(FOBJS) -o $(BIN)

install :: $(BIN)
	install -s $(BIN) $(INST_DIR)

clean ::
	rm -f $(BIN) $(FOBJS)
