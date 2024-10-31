#pragma once

#include "CoreMinimal"
#include "IDetailCustomization.h"

class FAutoLegFinder : public IDetailCustomization {

public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

protected:
	void AutoFindLegs();

private:
	
};