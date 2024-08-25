// Some copyright should be here...

#pragma once

namespace AnchorpointCliOperations
{
	TValueOrError<FString, FString> ANCHORPOINTCLI_API Connect();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API GetStatus();	
}
