/*
 Name:		Tetris_Using_UNO_R4_WIFI.ino
 Created:	2025/12/7 13:48:35
 Author:	Yuki Rin
*/

#include <Arduino.h>
#include <Arduino_LED_Matrix.h>
#include <gallery.h>
#include <TextAnimation.h>
#include <ArduinoGraphics.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <KeyboardLayout.h>
#include <Keyboard_da_DK.h>
#include <Keyboard_de_DE.h>
#include <Keyboard_es_ES.h>
#include <Keyboard_fr_FR.h>
#include <Keyboard_hu_HU.h>
#include <Keyboard_it_IT.h>
#include <Keyboard_pt_PT.h>
#include <Keyboard_sv_SE.h>
#include <HID.h>

ArduinoLEDMatrix matrix;

uint8_t newGrid[8][12] = { 0 };//当前落下方块的矩阵
uint8_t readyGrid[8][12] = { 0 };//准备落下方块的矩阵
uint8_t finalGrid[8][12] = { 0 };//最终固定方块的矩阵
uint8_t tempGrid[8][12] = { 0 };//临时矩阵

//随机生成方块函数
void randomGenerate() {
	int i = random(0, 7);//生成形状的随机数
	switch (i) {
	case 0://口字形
		readyGrid[0][5] = 1;
		readyGrid[0][6] = 1;
		readyGrid[1][5] = 1;
		readyGrid[1][6] = 1;
		break;
	case 1://一字形
		readyGrid[0][4] = 1;
		readyGrid[0][5] = 1;
		readyGrid[0][6] = 1;
		readyGrid[0][7] = 1;
		break;
	case 2://L字形
		readyGrid[0][5] = 1;
		readyGrid[1][5] = 1;
		readyGrid[2][5] = 1;
		readyGrid[2][6] = 1;
		break;
	case 3://J字形
		readyGrid[0][6] = 1;
		readyGrid[1][6] = 1;
		readyGrid[2][6] = 1;
		readyGrid[2][5] = 1;
		break;
	case 4://S字形
		readyGrid[0][5] = 1;
		readyGrid[0][6] = 1;
		readyGrid[1][6] = 1;
		readyGrid[1][7] = 1;
		break;
	case 5://Z字形
		readyGrid[0][7] = 1;
		readyGrid[0][6] = 1;
		readyGrid[1][6] = 1;
		readyGrid[1][5] = 1;
		break;
	case 6://T字形
		readyGrid[0][5] = 1;
		readyGrid[1][5] = 1;
		readyGrid[1][6] = 1;
		readyGrid[2][6] = 1;
		break;
	}
}

//方块下落函数
void moveDown() {
	for (int i = 7; i >= 0; i--) {
		for (int j = 0; j < 12; j++) {
			if (newGrid[i][j] == 1) {
				if (i == 7 || finalGrid[i + 1][j] == 1) {//到达底部或碰到固定方块
					//将当前方块写入最终矩阵
					for (int m = 0; m < 8; m++) {
						for (int n = 0; n < 12; n++) {
							if (newGrid[m][n] == 1) {
								finalGrid[m][n] = 1;
								newGrid[m][n] = 0;
							}
						}
					}
					//生成新方块
					memset(readyGrid, 0, sizeof(readyGrid));
					randomGenerate();
					memcpy(newGrid, readyGrid, sizeof(readyGrid));
					return;
				}
				else if (i < 7 && newGrid[i + 1][j] == 1) {
					newGrid[i][j] = 1;
					newGrid[i + 1][j] = 0;
				}
			}
		}
	}
}

// 游戏结束处理
void gameOver() {
	Serial.print("Game Over!");
	memset(finalGrid, 0, sizeof(finalGrid));
	memset(newGrid, 0, sizeof(newGrid));
	memset(readyGrid, 0, sizeof(readyGrid));
	randomGenerate();
	memcpy(newGrid, readyGrid, sizeof(readyGrid));
}

//初始化游戏状态
void GameStart() {
	Serial.print("Game Start!");
	memset(finalGrid, 0, sizeof(finalGrid));
	memset(newGrid, 0, sizeof(newGrid));
	memset(readyGrid, 0, sizeof(readyGrid));
	randomGenerate();
	memcpy(newGrid, readyGrid, sizeof(readyGrid));
}

bool millisDelay(unsigned long ms) {
	unsigned long start = millis();
	if (millis() - start < ms) {
		return false;
	}
	return true;
}

//读取输入控制方块移动
void control() {
	char inChar = '\0';
	if (millisDelay(500)) {
		inChar = Serial.read();
	}
	switch (inChar) {
	case 'a'://左移
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 12; j++) {
				if (newGrid[i][j] == 1) {
					if (j == 0 || finalGrid[i][j - 1] == 1) {
						return;
					}
				}
			}
		}
		for (int i = 0; i < 8; i++) {
			for (int j = 1; j < 12; j++) {
				if (newGrid[i][j] == 1) {
					newGrid[i][j - 1] = 1;
					newGrid[i][j] = 0;
				}
			}
		}
		break;
	case 'd'://右移
		for (int i = 0; i < 8; i++) {
			for (int j = 11; j >= 0; j--) {
				if (newGrid[i][j] == 1) {
					if (j == 11 || finalGrid[i][j + 1] == 1) {
						return;
					}
				}
			}
		}
		for (int i = 0; i < 8; i++) {
			for (int j = 10; j >= 0; j--) {
				if (newGrid[i][j] == 1) {
					newGrid[i][j + 1] = 1;
					newGrid[i][j] = 0;
				}
			}
		}
		break;
	case 's'://加速下落
		moveDown();
		break;
	}
}

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Serial.setTimeout(100);
	matrix.begin();
	Keyboard.begin();
	Mouse.begin();
	Serial.print("Initialized successfully");
}

void tempGridSet() {
	memset(tempGrid, 0, sizeof(tempGrid));
	for (int m = 0; m < 8; m++) {
		for (int n = 0; n < 12; n++) {
			if (finalGrid[m][n] == 1 || newGrid[m][n] == 1) {
				tempGrid[m][n] = 1;
			}
		}
	}
}

// the loop function runs over and over again until power down or reset
void loop() {
	GameStart();
	int i = 1;
	while (i == 1) {
		control();
		moveDown();
		//绘制矩阵
		matrix.clear();
		tempGridSet();
		for (int m = 0; m < 8; m++) {
			for (int n = 0; n < 12; n++) {
				if (finalGrid[m][n] == 1 || newGrid[m][n] == 1) {
					matrix.renderBitmap(tempGrid, m, n);
				}
			}
		}
		millisDelay(500);
		//检查游戏结束条件
		for (int n = 0; n < 12; n++) {
			if (finalGrid[0][n] == 1) {
				gameOver();
				i = 0;
				break;
			}
		}
	}
}