CC = C:\msys64\ucrt64\bin\gcc.exe
CFLAGS = -Wall -Wextra -O2 -I C:\msys64\ucrt64\include -I C:\msys64\ucrt64\include\ncurses
LDFLAGS = -L C:\msys64\ucrt64\lib -lncursesw

TARGET = editor.exe
SRC = editor.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	if exist $(TARGET) del /f $(TARGET)
