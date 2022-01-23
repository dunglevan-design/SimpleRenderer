#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <CanvasPoint.h>
#include <Colour.h>
#include <cmath>
#include <CanvasTriangle.h>
#include <glm/glm.hpp>
#include <TextureMap.h>
#include <TexturePoint.h>
#include <ModelTriangle.h>
#include <map>
#include <algorithm>
#include <RayTriangleIntersection.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
#define WIDTH 320
#define HEIGHT 240


std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
	std::vector<float> returnVector{ from };
	if (numberOfValues == 1) {
		return returnVector;
	}
	for (int i = 2; i <= numberOfValues; i++)
	{
		returnVector.push_back(from + ((to - from) / (numberOfValues - 1)) * (i - 1));
	}
	return returnVector;
}

std::vector<CanvasPoint> interpolateSinglePoints(CanvasPoint from, CanvasPoint to) {
	vector<CanvasPoint> returnvector{ from };

	float xDiff = from.x - to.x;
	float yDiff = from.y - to.y;
	float zDiff = from.depth - to.depth;
	int numberofsteps = round(max(max(abs(xDiff), abs(yDiff)), abs(zDiff))) + 1;


	vector<float> tests = interpolateSingleFloats(2.2, 8.5, 2);
	vector<float> xs = interpolateSingleFloats(from.x, to.x, numberofsteps);
	vector<float> ys = interpolateSingleFloats(from.y, to.y, numberofsteps);
	vector<float> zs = interpolateSingleFloats(from.depth, to.depth, numberofsteps);

	for (int i = 0; i < numberofsteps; i++)
	{
		CanvasPoint point = CanvasPoint(xs[i], ys[i], zs[i]);
		returnvector.push_back(point);
	}
	return returnvector;
}
std::vector<CanvasPoint> interpolateSinglePoints(CanvasPoint from, CanvasPoint to, int numberofsteps) {
	vector<CanvasPoint> returnvector{ from };

	//int numberOfSteps = std::max(abs(xDiff), abs(yDiff));


	vector<float> tests = interpolateSingleFloats(2.2, 8.5, 2);
	vector<float> xs = interpolateSingleFloats(from.x, to.x, numberofsteps);
	vector<float> ys = interpolateSingleFloats(from.y, to.y, numberofsteps);
	vector<float> zs = interpolateSingleFloats(from.depth, to.depth, numberofsteps);

	for (int i = 0; i < numberofsteps; i++)
	{
		CanvasPoint point = CanvasPoint(xs[i], ys[i], zs[i]);
		returnvector.push_back(point);
	}
	return returnvector;
}


vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues)
{
	vector<glm::vec3> result;

	vector<float> firstColumn = interpolateSingleFloats(from[0], to[0], numberOfValues);
	vector<float> secondColumn = interpolateSingleFloats(from[1], to[1], numberOfValues);
	vector<float> thirdColumn = interpolateSingleFloats(from[2], to[2], numberOfValues);

	for (size_t i = 0; i < numberOfValues; i++)
	{
		glm::vec3 resultvector = glm::vec3(firstColumn[i], secondColumn[i], thirdColumn[i]);
		result.push_back(resultvector);
	}
	return result;
}

void interpolate2D(DrawingWindow& window) {
	glm::vec3 topLeft(255, 0, 0);	   // red
	glm::vec3 topRight(0, 0, 255);	   // blue
	glm::vec3 bottomRight(0, 255, 0);  // green
	glm::vec3 bottomLeft(255, 255, 0); // yellow

	//interpolate first and last column
	vector<glm::vec3> firstcolumn = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
	vector<glm::vec3> lastcolumn = interpolateThreeElementValues(topRight, bottomRight, window.height);

	for (size_t i = 0; i < window.height; i++)
	{
		uint32_t colour = (255 << 24) + ((int)(firstcolumn.at(i).x) << 16) + ((int)(firstcolumn.at(i).y) << 8) + ((int)(firstcolumn.at(i).z));
		window.setPixelColour(0, i, colour);
	}

	for (size_t i = 0; i < window.height; i++)
	{
		uint32_t colour = (255 << 24) + ((int)(lastcolumn.at(i).x) << 16) + ((int)(lastcolumn.at(i).y) << 8) + ((int)(lastcolumn.at(i).z));
		window.setPixelColour(319, i, colour);
	}

	//interpolate rows

	for (size_t y = 0; y < window.height; y++)
	{
		glm::vec3 from = firstcolumn.at(y);
		glm::vec3 to = lastcolumn.at(y);
		vector<glm::vec3> row = interpolateThreeElementValues(from, to, window.width);

		for (size_t x = 0; x < window.width; x++)
		{
			uint32_t colour = (255 << 24) + ((int)(row.at(x).x) << 16) + ((int)(row.at(x).y) << 8) + ((int)(row.at(x).z));
			window.setPixelColour(x, y, colour);
		}
	}
}

void drawline(CanvasPoint from, CanvasPoint to, Colour c, DrawingWindow& window, std::vector<std::vector<float>>& z)
{
	std::vector<CanvasPoint> line = interpolateSinglePoints(from, to);
	for (int i = 0; i < line.size(); i++)
	{
		float depth = line[i].depth;
		int x = round(line[i].x);
		int y = round(line[i].y);
		if (0 > x || x >= WIDTH || 0 > y || y >= HEIGHT) continue;
		//plus 1 to shift all negatives to positive
		if (z[x][y] == 0 || 1 / depth > z[x][y]) { // -100
			z[x][y] = 1 / depth;
			uint32_t colour = (255 << 24) + ((int)(c.red) << 16) + ((int)(c.green) << 8) + ((int)(c.blue));
			window.setPixelColour(x, y, colour);
		}

		//uint32_t colour = (255 << 24) + ((int)(c.red) << 16) + ((int)(c.green) << 8) + ((int)(c.blue));
		//window.setPixelColour(round(line[i].x), round(line[i].y), colour);

	}
}


void drawTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow& window, std::vector<std::vector<float>>& z)
{
	drawline(triangle.v0(), triangle.v1(), colour, window, z);
	drawline(triangle.v1(), triangle.v2(), colour, window, z);
	drawline(triangle.v2(), triangle.v0(), colour, window, z);
}


map<string, Colour> LoadObjMaterial() {
	map<string, Colour> pallete;
	pallete["Red"] = Colour("Test", 255, 0, 0);
	ifstream inputStream("cornell-box.mtl", ifstream::binary);
	string nextline;
	string colourname;
	for (size_t i = 0; i < 23; i++)
	{
		getline(inputStream, nextline);
		if (nextline.empty()) {
			continue;
		}
		vector<string> arr = split(nextline, ' ');
		if (arr[0] == "newmtl") {
			colourname = arr[1];
		}
		if (arr[0] == "Kd") {
			int red = (round)(stof(arr[1]) * 255);
			int green = (round)(stof(arr[2]) * 255);
			int blue = (round)(stof(arr[3]) * 255);
			Colour colour = Colour(colourname, red, green, blue);
			pallete[colourname] = colour;
		}
	}

	return pallete;
}


vector<ModelTriangle> LoadObjtriangles() {
	vector<ModelTriangle> triangles;
	map<string, Colour>pallete = LoadObjMaterial();

	vector<glm::vec3> vs;
	float scaling = 0.17;
	ifstream inputStream("cornell-box.obj", std::ifstream::binary);
	string nextline;
	getline(inputStream, nextline);
	getline(inputStream, nextline);
	//skip comment line
	//119 = number of lines - first 2 comment lines.
	Colour c = Colour();
	for (size_t i = 0; i < 119; i++)
	{

		// get first object
		getline(inputStream, nextline);
		if (nextline.empty()) {
			continue;
		}
		// get first object

		else if (nextline.at(0) == 'o') {
			//object
			continue;
		}
		else if (nextline.at(0) == 'u') {
			//color
			vector<string> colorlinevector = split(nextline, ' ');
			c = pallete.find(colorlinevector[1])->second;

		}
		else if (nextline.at(0) == 'v') {
			//vectors or facets.
			vector<string> linevector = split(nextline, ' ');
			vs.push_back(glm::vec3(stof(linevector[1]) * scaling, stof(linevector[2]) * scaling, stof(linevector[3]) * scaling));
		}

		else if (nextline.at(0) == 'f') {
			vector<string> facetvalues = split(nextline, ' ');
			facetvalues[1].pop_back();
			facetvalues[2].pop_back();
			facetvalues[3].pop_back();
			int firstvalue = stoi(facetvalues[1]) - 1;
			int secondvalue = stoi(facetvalues[2]) - 1;
			int thirdvalue = stoi(facetvalues[3]) - 1;
			//Colour color = Colour("red", 255, 1, 1);
			ModelTriangle triangle = ModelTriangle(vs[firstvalue], vs[secondvalue], vs[thirdvalue], c);

			triangles.push_back(triangle);

		}
	}
	return triangles;
}

void drawPixel(int x, int y, Colour c, DrawingWindow& window) {
	uint32_t colour = (255 << 24) + ((int)(c.red) << 16) + ((int)(c.green) << 8) + ((int)(c.blue));
	window.setPixelColour(x, y, colour);
}


std::vector<CanvasPoint> SortByYcoordinate(CanvasTriangle triangle)
{
	std::vector<CanvasPoint> v;
	if (triangle.v0().y >= triangle.v1().y && triangle.v1().y >= triangle.v2().y)
	{
		v.push_back(triangle.v0());
		v.push_back(triangle.v1());
		v.push_back(triangle.v2());
	}

	else if (triangle.v0().y >= triangle.v2().y && triangle.v2().y >= triangle.v1().y)
	{
		v.push_back(triangle.v0());
		v.push_back(triangle.v2());
		v.push_back(triangle.v1());
	}

	else if (triangle.v1().y >= triangle.v0().y && triangle.v0().y >= triangle.v2().y)
	{
		v.push_back(triangle.v1());
		v.push_back(triangle.v0());
		v.push_back(triangle.v2());
	}
	else if (triangle.v1().y >= triangle.v2().y && triangle.v2().y >= triangle.v0().y)
	{
		v.push_back(triangle.v1());
		v.push_back(triangle.v2());
		v.push_back(triangle.v0());
	}

	else if (triangle.v2().y >= triangle.v0().y && triangle.v0().y >= triangle.v1().y)
	{
		v.push_back(triangle.v2());
		v.push_back(triangle.v0());
		v.push_back(triangle.v1());
	}

	else if (triangle.v2().y >= triangle.v1().y && triangle.v1().y >= triangle.v0().y)
	{
		v.push_back(triangle.v2());
		v.push_back(triangle.v1());
		v.push_back(triangle.v0());
	}
	return v;
}

CanvasPoint find(CanvasPoint top, CanvasPoint mid, CanvasPoint bottom)
{
	int extray = mid.y;
	int extrax = top.x + (mid.y - top.y) / (bottom.y - top.y) * (bottom.x - top.x);
	return CanvasPoint(extrax, extray);
}


void drawFilledTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow& window, std::vector<std::vector<float>>& z)
{
	drawTriangle(triangle, colour, window, z);
	std::vector<CanvasPoint> v = SortByYcoordinate(triangle);
	CanvasPoint top = v[2];
	CanvasPoint mid = v[1];
	CanvasPoint bottom = v[0];
	CanvasPoint extra = find(top, mid, bottom);

	float xDiffa = abs(top.x - mid.x);
	float yDiffa = abs(top.y - mid.y);
	float zDiffa = abs(top.depth - mid.depth);

	float xDiffb = abs(top.x - extra.x);
	float yDiffb = abs(top.y - extra.y);
	float zDiffb = abs(top.depth - extra.depth);

	int numberofPoints = round(std::max(((((xDiffa, yDiffa), zDiffa), xDiffb), yDiffb), zDiffb)) + 2;


	std::vector<CanvasPoint> froms = interpolateSinglePoints(top, mid, numberofPoints);
	std::vector<CanvasPoint> tos = interpolateSinglePoints(top, extra, numberofPoints);

	for (int i = 0; i < numberofPoints; i++)
	{
		drawline(froms[i], tos[i], colour, window, z);

	}

	float xdiffa = abs(bottom.x - extra.x);
	float ydiffa = abs(bottom.y - extra.y);
	float zdiffa = abs(bottom.depth - extra.depth);

	float xdiffb = abs(bottom.x - mid.x);
	float ydiffb = abs(bottom.y - mid.y);
	float zdiffb = abs(bottom.depth - mid.depth);

	int numberofpoints = round(max(((((xdiffa, ydiffa), zdiffa), xdiffb), ydiffb), zdiffb)) + 2;
	std::vector<CanvasPoint> Bottomfroms = interpolateSinglePoints(mid, bottom, numberofpoints);
	std::vector<CanvasPoint> Bottomtos = interpolateSinglePoints(extra, bottom, numberofpoints);

	for (size_t i = 0; i < numberofpoints; i++)
	{
		drawline(Bottomfroms[i], Bottomtos[i], colour, window, z);
	}

}



uint32_t findTexture(int x, int y, TextureMap img) {
	return img.pixels[y * 480 + x];
}
void drawTextureLine(CanvasPoint from, CanvasPoint to, DrawingWindow& window, TextureMap img) {
	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	if (xDiff == 0 && yDiff == 0) {
		return;
	}
	float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
	float xStepSize = xDiff / numberOfSteps;
	float xTextureStepSize = (to.texturePoint.x - from.texturePoint.x) / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;
	float yTextureStepSize = (to.texturePoint.y - from.texturePoint.y) / numberOfSteps;

	for (size_t i = 0; i <= numberOfSteps; i++)
	{
		if (i == numberOfSteps) {
			cout << "line 198";
		}
		float x = from.x + xStepSize * i;
		float xtexture = from.texturePoint.x + xTextureStepSize * i;


		float y = from.y + yStepSize * i;
		float ytexture = from.texturePoint.y + yTextureStepSize * i;

		uint32_t colour = findTexture(round(xtexture), round(ytexture), img);

		window.setPixelColour(round(x), round(y), colour);
	}
}

CanvasPoint findvk(CanvasPoint v0, CanvasPoint v1, CanvasPoint v2) {
	float ratio = (v1.y - v0.y) / (v2.y - v0.y);
	int vky = v1.y;
	int vkx = v0.x + ratio * (v2.x - v0.x);

	int Texturevkx = v0.texturePoint.x + ratio * (v2.texturePoint.x - v0.texturePoint.x);
	int Texturevky = v0.texturePoint.y + ratio * (v2.texturePoint.y - v0.texturePoint.y);
	return CanvasPoint(vkx, vky, Texturevkx, Texturevky);
}

void TextureMappingTriangle(DrawingWindow& window) {
	TextureMap img = TextureMap("texture.ppm");

	CanvasPoint v0 = CanvasPoint(160, 10, 195, 5);
	CanvasPoint v1 = CanvasPoint(10, 150, 65, 330);
	CanvasPoint v2 = CanvasPoint(300, 230, 395, 380);

	drawTextureLine(v0, v1, window, img);
	CanvasPoint vk = findvk(v0, v1, v2);
	//fill top triangle  (v0 -> v1/vk)
	float topmidDiffx = v0.x - v1.x;
	float topextraDiffx = v0.x - vk.x;
	float yDiff = v1.y - v0.y;
	int numberOfSteps = max(std::max(abs(topmidDiffx), abs(topextraDiffx)), abs(yDiff));

	float topmidStepSize = topmidDiffx / numberOfSteps;
	float topextraStepSize = topextraDiffx / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;


	float texturev0_vk_x_stepsize = (v0.texturePoint.x - vk.texturePoint.x) / numberOfSteps;
	float texturev0_vk_y_stepsize = (v0.texturePoint.y - vk.texturePoint.y) / numberOfSteps;
	float texturev0_v1_x_stepsize = (v0.texturePoint.x - v1.texturePoint.x) / numberOfSteps;
	float texturev0_v1_y_stepsize = (v0.texturePoint.y - v1.texturePoint.y) / numberOfSteps;


	for (size_t i = 0; i <= numberOfSteps; i++)
	{
		int TexturePointFromX = round(v0.texturePoint.x - texturev0_v1_x_stepsize * i);
		int TexturePointFromY = round(v0.texturePoint.y - texturev0_v1_y_stepsize * i);
		CanvasPoint from = CanvasPoint(v0.x - topmidStepSize * i, v0.y + yStepSize * i, TexturePointFromX, TexturePointFromY);
		CanvasPoint to = CanvasPoint(v0.x - topextraStepSize * i, v0.y + yStepSize * i, (int)round(v0.texturePoint.x - texturev0_vk_x_stepsize * i), (int)round(v0.texturePoint.y - texturev0_vk_y_stepsize * i));
		if (i == numberOfSteps - 1) {
			cout << "line 253";
		}
		drawTextureLine(from, to, window, img);
	}

	//fill bottom triangle (v1, vk -> v2)
	float extrabottomDiffx = vk.x - v2.x;
	float midbottomDiffx = v1.x - v2.x;
	float ydiff = v2.y - vk.y;

	float ydifftexturepoint = std::max(abs(v2.texturePoint.y - vk.texturePoint.y), abs(v2.texturePoint.y - v1.texturePoint.y));
	float xdifftexturepoint = std::max(abs(v2.texturePoint.x - vk.texturePoint.x), abs(v2.texturePoint.x - v1.texturePoint.x));
	float numberOfsteps = max(max(max(std::max(abs(extrabottomDiffx), abs(midbottomDiffx)), abs(ydiff)), ydifftexturepoint), xdifftexturepoint);

	float extrabottomStepSize = extrabottomDiffx / numberOfsteps;
	float midbottomStepSize = midbottomDiffx / numberOfsteps;
	float ystepSize = ydiff / numberOfsteps;

	float texturev1_v2_x_stepsize = (v1.texturePoint.x - v2.texturePoint.x) / numberOfsteps;
	float texturev1_v2_y_stepsize = (v1.texturePoint.y - v2.texturePoint.y) / numberOfsteps;
	float texturevk_v2_x_stepsize = (vk.texturePoint.x - v2.texturePoint.x) / numberOfsteps;
	float texturevk_v2_y_stepsize = (vk.texturePoint.y - v2.texturePoint.y) / numberOfsteps;


	for (size_t i = 0; i <= numberOfsteps; i++)
	{
		CanvasPoint From = CanvasPoint(v1.x - midbottomStepSize * i, v1.y + ystepSize * i, (int)round(v1.texturePoint.x - texturev1_v2_x_stepsize * i), (int)round(v1.texturePoint.y - texturev1_v2_y_stepsize * i));
		CanvasPoint To = CanvasPoint(vk.x - extrabottomStepSize * i, vk.y + ystepSize * i, (int)round(vk.texturePoint.x - texturevk_v2_x_stepsize * i), (int)round(vk.texturePoint.y - texturevk_v2_y_stepsize * i));
		drawTextureLine(From, To, window, img);
	}
	//

}




glm::vec3 getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::vec3 vertexPosition, float focalLength, glm::mat3 cameraOrientation) {
	float scalex = 300;
	float scaley = 300;
	float scalez = 300;
	glm::vec3 scaledvertexPosition = glm::vec3(vertexPosition.x * scalex, vertexPosition.y * scaley, vertexPosition.z);

	//glm::vec3 v = scaledvertexPosition - cameraPosition;
	glm::vec3 v = (vertexPosition - cameraPosition) * cameraOrientation;

	float x = focalLength * scalex * v.x / v.z + WIDTH / 2;
	float y = focalLength * scaley * v.y / v.z + HEIGHT / 2;
	float z = -scalez * v.z;
	return glm::vec3(WIDTH - x, y, z);
}

void draw3D(DrawingWindow& window, std::vector<std::vector<float>>& z, glm::vec3& cameraPosition, float& focalLength, vector<ModelTriangle> triangles, map<string, Colour>pallete, glm::mat3 cameraOrientation) {
	//vector<ModelTriangle> triangles = LoadObjtriangles(window);
	//map<string, Colour>pallete = LoadObjMaterial(window);


	for (size_t i = 0; i < triangles.size(); i++)
	{
		//cout << triangles[i].vertices[0].x << endl;
		glm::vec3 v0 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[0], focalLength, cameraOrientation);
		glm::vec3 v1 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[1], focalLength, cameraOrientation);
		glm::vec3 v2 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[2], focalLength, cameraOrientation);
		CanvasTriangle canvastriangle = CanvasTriangle(CanvasPoint(v0.x, v0.y, v0.z), CanvasPoint(v1.x, v1.y, v1.z), CanvasPoint(v2.x, v2.y, v2.z));
		drawFilledTriangle(canvastriangle, triangles[i].colour, window, z);
	}
}


void draw(DrawingWindow& window, glm::vec3& cameraPosition, float& focalLength, vector<ModelTriangle> triangles, map<string, Colour>pallete, glm::mat3& cameraOrientation) {
	window.clearPixels();
	std::vector<std::vector<float>> z(WIDTH, std::vector<float>(HEIGHT, 0));
	draw3D(window, z, cameraPosition, focalLength, triangles, pallete, cameraOrientation);

}


void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) {	
			std::cout << "LEFT" << std::endl;
		} 
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;


	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	vector<ModelTriangle> triangles = LoadObjtriangles();
	map<string, Colour>pallete = LoadObjMaterial();
	
	glm::vec3 cameraPosition = glm::vec3(0, 0, 4.0);
	glm::mat3 cameraOrientation(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
	float focalLength = 2.0;

	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	while (true) {
		draw(window, cameraPosition, focalLength, triangles, pallete, cameraOrientation);
		// We MUST poll for events - otherwise the window will freeze !

		if (window.pollForInputEvents(event)) handleEvent(event, window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
