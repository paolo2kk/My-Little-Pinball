#include "Text.h"

Text::Text()
{
	first_character = (char)0;
	character_size = 0;
	columns = 0;
	rows = 0;
}
Text::~Text()
{
}
void Text::Initialise(const char* file_path, char first_character, int character_size)
{
	img = LoadTexture(file_path);

	this->first_character = first_character;
	this->character_size = character_size;

	//colums and rows
	columns = img.width / character_size;
	rows = img.height / character_size;
}
void Text::Draw(int x, int y, const std::string& text, const Color& col) const
{
	//Offset so that when whe write the firs character we dont overwrite it with another
	int offset_x = x;

	for (int i = 0; i < text.length(); ++i)
	{
		DrawCharacter(offset_x, y, text[i], col);

		offset_x += character_size;

	}
}
void Text::DrawCharacter(int x, int y, char c, const Color& col) const
{
	int char_index, coord_x, coord_y, pixel_x, pixel_y;

	//Get The cheracter
	char_index = c - first_character;

		coord_x = char_index % columns;
		coord_y = char_index / columns;

	//The cordenates on the map
	pixel_x = coord_x * character_size;
	pixel_y = coord_y * character_size;

	Rectangle rc = { (float)pixel_x, (float)pixel_y, (float)character_size, (float)character_size };

	//Draw the character
	DrawTextureRec(img, rc, { (float)x, (float)y }, col);
}