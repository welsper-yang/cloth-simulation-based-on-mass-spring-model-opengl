// ClothSimulation.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "camera.h"
#include "simulation.h"
#include "shaders.h"
#include "parameter.h"
#include "model.h"
#include "ui.h"
#include "settings.h"

#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

Camera camera;
LoadSettings loadSettings;
Simulation simulation;
Model model;
Parameters parameters;
Shaders shaders;
TweakUI tweakUI;

//Start UI Events Area.
void TW_CALL startOrPause(void *clientData) {
	tweakUI.startOrPause(clientData);
}

void TW_CALL ifFillTexture(void *clientData) {
	tweakUI.ifFillTexture(clientData);
}

void TW_CALL ifWind(void *clientData) {
	tweakUI.ifWind(clientData);
}

void TW_CALL reset(void *clientData) {
	tweakUI.reset(clientData);
	tweakUI.getSimulation()->getParameters()->resetFixedPoints();
}

void TW_CALL releasePin1(void *clientData) {
	tweakUI.getSimulation()->getParameters()->fixedPoints[0] = -10.0f;
}

void TW_CALL releasePin2(void *clientData) {
	tweakUI.getSimulation()->getParameters()->fixedPoints[1] = -10.0f;
}

void TW_CALL releasePin3(void *clientData) {
	tweakUI.getSimulation()->getParameters()->fixedPoints[2] = -10.0f;
}

void TW_CALL releasePin4(void *clientData) {
	tweakUI.getSimulation()->getParameters()->fixedPoints[3] = -10.0f;
}
//End UI Events Area.

void initAllObjects() {
	model.initModel(loadSettings.width, loadSettings.height, loadSettings.clothWidth, loadSettings.clothHeight, 90.0, glm::vec4(0.0, 0.0, 0.0, 1.0));
	model.loadParticles();
	parameters.imageTexturePath = loadSettings.imagePath;
	parameters.fixedPointsCopy[0] = parameters.fixedPoints[0] = loadSettings.fixedPoints[0];
	parameters.fixedPointsCopy[1] = parameters.fixedPoints[1] = loadSettings.fixedPoints[1];
	parameters.fixedPointsCopy[2] = parameters.fixedPoints[2] = loadSettings.fixedPoints[2];
	parameters.fixedPointsCopy[3] = parameters.fixedPoints[3] = loadSettings.fixedPoints[3];

	shaders.bindModel(&model);
	shaders.bindParameters(&parameters);
	shaders.createFBOAndTextures();
	shaders.initializeFBO();
	shaders.loadShaders();

	simulation.bindCamera(&camera);
	simulation.bindModel(&model);
	simulation.bindParameters(&parameters);
	simulation.bindShaders(&shaders);

	tweakUI.bindSimulation(&simulation);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glLoadIdentity();
	gluLookAt(camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z, 0, 0, 0, 0, 1, 0);
	glColor3f(1.0f, 0.0f, 0.0f);

	tweakUI.drawUI();

	//simulation.drawFloor();
	simulation.simulation();

	glutSwapBuffers();
}

void animation() {
	glutPostRedisplay();
}

void reshape(int w, int h) {
	camera.windowsReshape(w, h);

	TwWindowSize(w, h);
}

void keyboardPush(unsigned char key, int x, int y) {
	camera.keyboardPush(key, x, y);
}

void mouseClick(int button, int state, int x, int y) {
	camera.mouseClick(button, state, x, y);
}

void mouseMove(int x, int y) {
	camera.mouseMove(x, y);
}

void mouseWheel(int button, int direction, int x, int y) {
	camera.mouseWheel(button, direction, x, y);
}

int main(int argc, char**argv)
{
	TwBar *bar;//Tweak Bar Object for UI.

	loadSettings.loadSettings();//Load Options.

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Simulation");
	glewInit();

	TwInit(TW_OPENGL, NULL);//Init Tweak UI.

	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardPush);
	//glutMouseFunc(mouseClick);
	//glutMotionFunc(mouseMove);
	glutMouseWheelFunc(mouseWheel);
	
	glutDisplayFunc(display);
	glutIdleFunc(animation);

	initAllObjects();

	//Begin UI Settings.
	tweakUI.initUIFunction();
	bar = TwNewBar("Options");
	TwDefine("GLOBAL help='Details about cloth'");
	TwDefine("Options size='300 400' color='188 92 197'");
	TwAddSeparator(bar, "SeparatorDetail", "group='Detail'");
	TwAddVarRW(bar, "Particles Number", TW_TYPE_INT32, &tweakUI.getSimulation()->getModel()->totalParticles, "group='Detail' step = 0");
	TwAddVarRW(bar, "Damping", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->Damping, "group='Detail' step = 0");
	TwAddVarRW(bar, "Ks Struct", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->KsStruct, "group='Detail' step = 0");
	TwAddVarRW(bar, "Kd Struct", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->KdStruct, "group='Detail' step = 0");
	TwAddVarRW(bar, "Ks Shear", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->KsShear, "group='Detail' step = 0");
	TwAddVarRW(bar, "Kd Shear", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->KdShear, "group='Detail' step = 0");
	TwAddVarRW(bar, "Ks Bend", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->KsBend, "group='Detail' step = 0");
	TwAddVarRW(bar, "Kd Bend", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->KdBend, "group='Detail' step = 0");
	TwAddVarRW(bar, "Mass", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->Mass, "group='Detail' step = 0");
	TwAddSeparator(bar, "SeparatorFunction", "group='Function'");
	TwAddButton(bar, "Start", startOrPause, NULL, "label='Start/Pause' group='Function'");
	TwAddButton(bar, "Fill", ifFillTexture, NULL, "label='Show Grids' group='Function'");
	TwAddButton(bar, "Wind", ifWind, NULL, "label='Wind' group='Function'");
	TwAddButton(bar, "Reset", reset, NULL, "label='Reset' group='Function'");
	TwAddSeparator(bar, "SeparatorParameters", "group='Parameters'");
	TwAddVarRW(bar, "LightColor", TW_TYPE_COLOR3F, &tweakUI.getSimulation()->getParameters()->LightColor, "group='Parameters'");
	TwAddVarRW(bar, "TimeStep", TW_TYPE_FLOAT, &tweakUI.getSimulation()->getParameters()->TimeStep, "group='Parameters' min=0.005 max=0.085 step=0.005");
	TwAddSeparator(bar, "SeparatorDynamic", "group='Dynamic'");
	TwAddButton(bar, "Pin1", releasePin1, NULL, "label='Pin1' group='Dynamic'");
	TwAddButton(bar, "Pin2", releasePin2, NULL, "label='Pin2' group='Dynamic'");
	TwAddButton(bar, "Pin3", releasePin3, NULL, "label='Pin3' group='Dynamic'");
	TwAddButton(bar, "Pin4", releasePin4, NULL, "label='Pin4' group='Dynamic'");
	TwAddSeparator(bar, "SeparatorRender", "group='Render'");
	TwAddVarRW(bar, "IterationNumber", TW_TYPE_INT8, &tweakUI.getSimulation()->getParameters()->iterationNumber, "group='Render' min = 1 max = 50 step = 1");
	//End UI Settings.

	glutMainLoop();

    return 0;
}

