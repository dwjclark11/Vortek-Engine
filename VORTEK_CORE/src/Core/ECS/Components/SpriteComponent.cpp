#include "SpriteComponent.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include <Logger/Logger.h>

using namespace VORTEK_RESOURCES;

//void VORTEK_CORE::ECS::SpriteComponent::generate_uvs( int textureWidth, int textureHeight )
//{
//	uvs.uv_width = width / textureWidth;
//	uvs.uv_height = height / textureHeight;
//
//	uvs.u = start_x * uvs.uv_width;
//	uvs.v = start_y * uvs.uv_height;
//}

std::string VORTEK_CORE::ECS::SpriteComponent::to_string() const
{
	std::stringstream ss;
	ss << "==== Sprite Component ==== \n"
		<< std::boolalpha << "Texture Name: " << texture_name << "\n"
		<< "Width: " << width << "\n"
		<< "Height: " << height << "\n"
		<< "StartX: " << start_x << "\n"
		<< "StartY: " << start_y << "\n"
		<< "Layer: " << layer << "\n"
		<< "UVs: \n\t"
		<< "U: " << uvs.u << "\n\t"
		<< "V: " << uvs.v << "\n\t"
		<< "UvWidth: " << uvs.uv_width << "\n\t"
		<< "UvHeight: " << uvs.uv_width << "\n"
		<< "Color: \n\t"
		<< "Red: " << color.r << "\n\t"
		<< "Green: " << color.g << "\n\t"
		<< "Blue: " << color.b << "\n\t"
		<< "Alpha: " << color.a << "\n"
		<< "bHidden: " << bHidden << "\n";

	return ss.str();
}

void VORTEK_CORE::ECS::SpriteComponent::CreateSpriteLuaBind(sol::state& lua)
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	lua.new_usertype<VORTEK_RENDERING::Color>("Color",
		sol::call_constructor,
		sol::factories([](GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
			return VORTEK_RENDERING::Color{ .r = r, .g = g, .b = b, .a = a };
			}),
		"r",
		&VORTEK_RENDERING::Color::r,
		"g",
		&VORTEK_RENDERING::Color::g,
		"b",
		&VORTEK_RENDERING::Color::b,
		"a",
		&VORTEK_RENDERING::Color::a);

	lua.new_usertype<UVs>("UVs",
		sol::call_constructor,
		sol::factories([](float u, float v) { return UVs{ .u = u, .v = v }; }),
		"u",
		&UVs::u,
		"v",
		&UVs::v,
		"uv_width",
		&UVs::uv_width,
		"uv_height",
		&UVs::uv_height);

	lua.new_usertype<SpriteComponent>(
		"Sprite",
		"type_id",
		&entt::type_hash<SpriteComponent>::value,
		sol::call_constructor,
		sol::factories(
			[](const std::string& textureName, float width, float height, int start_x, int start_y, int layer) {
				return SpriteComponent{ .width = width,
										.height = height,
										.uvs = UVs{},
										.color = VORTEK_RENDERING::Color{ 255, 255, 255, 255 },
										.start_x = start_x,
										.start_y = start_y,
										.layer = layer,
										.texture_name = textureName };
			}),
		"texture_name",
		&SpriteComponent::texture_name,
		"width",
		&SpriteComponent::width,
		"height",
		&SpriteComponent::height,
		"start_x",
		&SpriteComponent::start_x,
		"start_y",
		&SpriteComponent::start_y,
		"layer",
		&SpriteComponent::layer,
		"bHidden",
		&SpriteComponent::bHidden,
		"uvs",
		&SpriteComponent::uvs,
		"color",
		&SpriteComponent::color,
		"generate_uvs",
		[&](SpriteComponent& sprite) {
			auto pTexture = assetManager.GetTexture(sprite.texture_name);

			if (!pTexture)
			{
				VORTEK_ERROR("Failed to generate uvs -- Texture [{}] -- Does not exists or invalid",
					sprite.texture_name);
				return;
			}

			VORTEK_CORE::GenerateUVs(sprite, pTexture->GetWidth(), pTexture->GetHeight());
		},
		"inspect_uvs",
		[](SpriteComponent& sprite) {
			sprite.uvs.u = sprite.start_x * sprite.uvs.uv_width;
			sprite.uvs.v = sprite.start_y * sprite.uvs.uv_height;
		},
		"inspect_x",
		[](SpriteComponent& sprite) { sprite.uvs.u = sprite.start_x * sprite.uvs.uv_width; },
		"inspect_y",
		[](SpriteComponent& sprite) { sprite.uvs.v = sprite.start_y * sprite.uvs.uv_height; });
}