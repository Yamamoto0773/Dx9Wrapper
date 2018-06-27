#include "Texture.hpp"

dx9::Texture::Texture() : clipInfo() {
}

dx9::Texture::~Texture() {
}

bool dx9::Texture::operator!() const {
	return !tex;
}
