#pragma once

#include "DX9ShareContainer.hpp"

namespace dx9 {

	namespace font {

		class DirectXFontBase : public resource::DX9ShareContainer {

		protected:
			HFONT					hFont;			// �ݒ蒆�̃t�H���g�̃n���h��
			AntialiasLevel			antialiasLv;

			size_t fontSize;

			std::array<float, 4> fontColor;

			int letterSpace;

			float charTravelAngle_rad; // travel direction of character (rad)

			FontRotOrigin fontRotOrigin;
			TextAlign textAlign;
			TextureAdjust strAdjust;

			DirectXFontBase();
			~DirectXFontBase();

			void Clear();
			
		public:

			// ARGB�̏�
			void SetFontColor(const Color &color);


			// ���Ԃ̎w�� pixel�P�ʂŎw��
			void SetLetterSpace(int size) { letterSpace = size; };

			// �������i�ޕ����̎w�� degree�Ŏw��
			void SetCharTravelDirection(int deg);
			// �������i�ޕ����̎w�� radian�Ŏw��
			void SetCharTravelDirection(float rad);

			// �����񂹂̐ݒ�
			void SetAlignment(dx9::TextAlign align) { textAlign = align; };

			// ������]�̒��S��ݒ�
			void SetRotateOrigin(FontRotOrigin origin) { fontRotOrigin = origin; };

			// ������̊g��k���Ɋւ���ݒ�
			// note:�t���O��NONE�ȊO�̏ꍇ�́C��s�����`�悳��܂���
			void SetStringAdjust(TextureAdjust adjust);

		};

	}
	
}