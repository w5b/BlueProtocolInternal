namespace esp
{
	struct EspInfo
	{
		FVector2D feetPos;
		FVector2D HeadPos;
		int width, height;
		int x, y;
	};


	void draw(UCanvas* pCanvas);
	bool getEspInfo(AASCharacterBase* CurrentActor, EspInfo* outEspInfo);
};