using namespace CG;

namespace drawing
{
	void myDrawText(UCanvas* pCanvas, UFont* font, FString text, FVector2D position, FVector2D scale, FLinearColor color)
	{
		FLinearColor shadow{ 0.f, 0.f, 0.f, 0.f };
		FLinearColor outline{ 0.f, 0.f, 0.f, 1.f };
		pCanvas->K2_DrawText(font, text, position, scale,
			color,
			1.f,
			shadow,
			scale,
			true,
			true,
			true,
			outline);
	}

	void DrawBox(CG::UCanvas* pCanvas, CG::FVector2D initial_pos, float w, float h, CG::FLinearColor color, float thickness)
	{
		pCanvas->K2_DrawLine(CG::FVector2D(initial_pos.X, initial_pos.Y), CG::FVector2D(initial_pos.X + w, initial_pos.Y), thickness, color);
		pCanvas->K2_DrawLine(CG::FVector2D(initial_pos.X, initial_pos.Y), CG::FVector2D(initial_pos.X, initial_pos.Y + h), thickness, color);
		pCanvas->K2_DrawLine(CG::FVector2D(initial_pos.X + w, initial_pos.Y), CG::FVector2D(initial_pos.X + w, initial_pos.Y + h), thickness, color);
		pCanvas->K2_DrawLine(CG::FVector2D(initial_pos.X, initial_pos.Y + h), CG::FVector2D(initial_pos.X + w, initial_pos.Y + h), thickness, color);
	}

	void DrawDistance(UCanvas* pCanvas, FVector2D Pos, int _distance, FLinearColor color)
	{
		std::wstring distance = L"[" + std::to_wstring(_distance) + L"]";
		const wchar_t* wDistance = distance.c_str();
		myDrawText(pCanvas, globals::defaultFont, wDistance, Pos, { 1.f, 1.f }, color);
	}

	void DrawPlayerName(UCanvas* pCanvas, FVector2D Pos, ASBPlayerCharacter* CurrentPlayer, FLinearColor color)
	{
		const FString& name = CurrentPlayer->PlayerState->PlayerNamePrivate;
		myDrawText(pCanvas, globals::defaultFont, name, Pos, { 1.f, 1.f }, color);
	}

	void DrawMonsterName(UCanvas* pCanvas, FVector2D Pos, ASBEnemyCharacter* CurrentEnemy, FLinearColor color)
	{
		const FString& name = CurrentEnemy->CharacterName;
		myDrawText(pCanvas, globals::defaultFont, name, Pos, { 1.f, 1.f }, color);
	}
	template <typename T>
	void DrawBones(UCanvas* pCanvas, std::vector<std::vector<T>> bones, ACharacter* player)
	{
		for (auto& bonearrays : bones)
		{
			FVector2D previousBone{};
			for (auto bone_index : bonearrays)
			{
				FTransform test = player->Mesh->GetSocketTransform(
					player->Mesh->GetBoneName(static_cast<int32_t>(bone_index)), ERelativeTransformSpace::RTS_World);
				FVector2D bonepostion2d;
				globals::MyController->ProjectWorldLocationToScreen(
					test.Translation, &bonepostion2d, true);
				if (bonepostion2d.X == 0.0f && bonepostion2d.Y == 0.0f)
					break;
				if (previousBone.X == 0.0f)
				{
					previousBone = bonepostion2d;
					continue;
				}
				pCanvas->K2_DrawLine(previousBone, bonepostion2d, 1.0, colors::White);
				/*pCanvas->K2_DrawText(defaultFont,
					FString(s2wc(player->Mesh->GetBoneName(static_cast<int32_t>(bone_index)).GetName().c_str())),
					bonepostion2d,
					FVector2D(0.5f, 0.5f),
					FLinearColor(0, 28, 83, 150),
					1.5f,
					FLinearColor(0, 0, 0, 0),
					FVector2D(-1.5f, 0.5f),
					true,
					true,
					true,
					FLinearColor(0, 0, 0, 150));*/
				previousBone = bonepostion2d;
			}
		}
	}

	void DrawHealthBar(UCanvas* pCanvas, float Health, float MaxHealth, FVector2D Position, FVector2D Size, float Thickness)
	{
		float perf = Health / MaxHealth;
		float OutOf = Size.Y * perf;
		pCanvas->K2_DrawLine(Position, { Position.X , Position.Y + Size.Y }, Thickness, colors::White);
		pCanvas->K2_DrawLine({ Position.X, Position.Y + Size.Y }, { Position.X, Position.Y + Size.Y - OutOf }, Thickness, colors::Green);
	}
}