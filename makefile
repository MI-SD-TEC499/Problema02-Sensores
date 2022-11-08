
#execução dos executaveis
all: sudo ./sensores

# compilação e link com o código da lib
build: raspberry.c
  gcc raspberry.c lcd.c lcd.h -o sensores -lwiringPi
