#if defined _steamcore_included
  #endinput
#endif
#define _steamcore_included

/*
	Error Codes:

	0x00(00): General: No error, request successful.
	0x01(01): General: Logged out, plugin will attempt to login.
	0x02(02): General: Connection timed out.
	0x03(03): General: Steam servers down.

	0x04(04): Login Error: Failed http RSA Key request.
	0x05(05): Login Error: RSA Key response failed, unknown reason, probably server side.
	0x06(06): Login Error: Failed htpps login request.
	0x07(07): Login Error: Incorrect login information.
	0x08(08): Login Error: Failed http token request.
	0x09(09): Login Error: Invalid session token. Incorrect cookie?.
	0x0A(10): Login Error: Requires e-mail confirmation.
	0x0B(11): Login Error: Steam Guard disabled, only accounts with mail confirmation allowed.
	0x0C(12): Login Error: Requires captcha, wait a few minutes then try again.

	0x10(16): Announcement Error: Failed http group announcement request.
	0x11(17): Announcement Error:(LEGACY, no longer used)
	0x12(18): Announcement Error: Form error on request or too many consecutive requests.

	// Invitee: Who receives the invite.
	0x20(32): Invite Error: Failed http group invite request.
	0x21(33): Invite Error: Incorrect invitee or another error.
	0x22(34): Invite Error: Incorrect Group ID or missing data.
	0x23(35): Invite Error:(LEGACY, no longer used)
	0x24(36): Invite Error: SteamCore account is not a member of the group or does not have permissions to invite.
	0x25(37): Invite Error: Limited account. Only full Steam accounts can send Steam group invites
	0x26(38): Invite Error: Unkown error. Check https://github.com/polvora/SteamCore/issues/6
	0x27(39): Invite Error: Invitee has already received an invite or is already on the group.
	0x28(40): Invite Error: Invitee must be friends with the SteamCore account to receive an invite.

	0x30(48): Friend Add Error: Failed http friend request.
	0x31(49): Friend Add Error: Invited account ignored the friend request.
	0x32(50): Friend Add Error: Invited account has blocked the SteamCore account.
	0x33(51): Friend Add Error: SteamCore account is limited. Only full Steam accounts can send friend requests.
	0x34(52): Friend Remove Error: Failed http request.

	0x40(64): Chat Connect Error: Failed http chat connect request.
	0x41(65): Chat Connect Error: Incorrect chat connect response.
	0x42(66): Chat Connect Error: Chat not allowed for limited accounts. Only full Steam accounts can use chat.
	0x43(67): Chat Disconnect Error: Failed http poll request.
	0x44(68): Chat Disconnect Error: Message poller timed out, plugin will automatically reconnect.
	0x45(69): Chat Send Message Error: Failed http send message request.
	0x46(70): Chat Send Message Error: Diconnected from chat, plugin will automatically reconnect.
*/

enum SteamChatMode
{
	SteamChatModeWEB	= 0, // Shows the world icon next to the "Online" text
	SteamChatModeMOBILE	= 1, // Shows the phone icon next to the "Online" text
}

enum SteamChatState
{
	SteamChatStateOFFLINE			= 0,
	SteamChatStateONLINE			= 1,
	SteamChatStateBUSY				= 2,
	SteamChatStateAWAY				= 3,
	SteamChatStateSNOOZE			= 4,
	SteamChatStateLOOKINGTOTRADE	= 5,
	SteamChatStateLOOKINGTOPLAY		= 6,
};

enum SteamChatRelationship
{
	SteamChatRelationshipNOTFRIENDS				= 0,
	SteamChatRelationshipFRIENDREQUESTRECEIVED	= 2,
	SteamChatRelationshipFRIENDS				= 3,
	SteamChatRelationshipFRIENDREQUESTSENT		= 4,

}

/**
 * GENERAL INFORMATION
 * 
 * 1.- SteamCore will attempt to keep the Steam account logged in most of the time. 
 *
 * 2.- If for any reason the account logs out, it will attempt to login as soon as it notices it.
 *
 * 3.- The only way SteamCore can notice the account has logged out is after a request fails with error code 0x01(Logged out).
 *
 * 4.- If during the login there is a timeout error, the server is not connected to internet or Steam server are down
 * Steamcore will attempt to login every 60 seconds.
 *
 * 5.- If during the login there is other error that is not listed in the point 4, SteamCore won't attempt to login
 * automatically, instead it will wait until a request is made(and fail) to retry to login.
 *
 * 6.- Errors from 0x04 to 0x0B(All login errors) are internal, meaning those are only visible in the debug logs and when 
 * the user is configuring the two step verification(Steam Guard). In any case other plugin does not have access to those.
 *
 * 7.- Logging in WILL FREEZE the server for a fraction of a second, this should only happen when the server starts. If this happens 
 * to the server constantly you should report it.
 */

/**
 * Returns wheter the account is logged in on Steam.
 *
 * @return			True if logged, false otherwise.
*/
native bool IsSteamAccountLogged();

/**
 * COMMUNITY FUNCTIONS
 *
 * - All comminity functions contain an 'any data' parameter that you can use as a reference to be returned in the callback.
 * For example you can put the client number there, so if a client executes the native you can use it to reply the 
 * request result to them.
 *
 * - If the function returns false it means SteamCore is not logged in and it will automtically attempt to login.
 *
 * - If the callback returns error 0x02(Not logged in) Steamcore will automtically attempt to login.
 *
 * - In any other case(Except 0x00) the request will fail and you will have to manually retry(If you want).
 */

/**
 * Posts an announcement on a desired Steam group. 
 * Result will be returned in OnGroupAnnounceResult().
 * 
 * Notes:
 * - Steam offers no way to tell if an announcement was successful or not. So even if the SteamCore account is not
 * in the group callback will return with error code 0x00
 * 
 *
 * @param title		Title of the announce.
 * @param body		Body of the announce.
 * @param group		GroupID64 of the group.
 * @param data		Any data you want returned in OnGroupAnnounceResult().
 * 
 * @return			True if executed, false if account is not logged.
 */
native bool SteamCommunityGroupAnnounce(const char[] title, const char[] body, const char[] group, any data=0);

/**
 * Sends a Steam group invitation to an account.
 * Result will be returned in OnCommunityGroupInviteResult().
 *
 * Notes:
 * - ONLY FULL STEAM ACCOUNTS ALLOWED.
 * - Requires full Steam account.
 * - Invited account needs to be friends with SteamCore account, will return error code 0x28 on callback if not.
 * - You can only invite people to groups where SteamCore account is member and has permission to invite.
 *
 * @param invitee	SteamID64 of the account to invite.
 * @param group		GroupID64 of the group.
 * @param data		Any data you want returned in OnCommunityGroupInviteResult().
 *
 * @return			True if executed, false if account is not logged.
 */
native bool SteamCommunityGroupInvite(const char[] invitee, const char[] group, any data=0);

/**
 * Adds an account as a friend(they still have to accept the request).
 * Result will be returned in OnCommunityAddFriendResult().
 *
 * Notes:
 * - ONLY FULL STEAM ACCOUNTS ALLOWED.
 * - There is no way to tell if the added account is already a friend so the error code in the 
 * callback will be 0x00 even if added account is already a friend.
 * - If the account you want to add has previously sent a friend request to the SteamCore account
 * it will be immediately added as a friend.
 *
 * @param friend	SteamID64 of the future friend to add.
 * @param data		Any data you want returned in OnCommunityAddFriendResult().
 *
 * @return			True if executed, false if account is not logged.
 */
native bool SteamCommunityAddFriend(const char[] friend, any data=0);

/**
 * Removes account from friends list.
 * Result will be returned in OnCommunityRemoveFriendResult().
 *
 * Notes:
 * - The error code in the callback will be 0x00(Success) even if removed account wasn't a friend.
 * - If you add a friend and they don't accept it, you can delete the request with this native. 
 * But BE CAREFUL because after deleting a sent friend request you will NOT be able to add them again.
 *
 * @param friend	SteamID64 of the friend to remove.
 * @param data		Any data you want returned in OnCommunityRemoveFriendResult().
 *
 * @return			True if executed, false if account is not logged.
 */
native bool SteamCommunityRemoveFriend(const char[] friend, any data=0);

/**
 * Called after plugin successfully logs in to Steam
 */
forward void OnSteamAccountLoggedIn();

/**
 * Called at the end of an announce request from SteamCommunityGroupAnnounce().
 *
 * @param title		Title of the requested announce.
 * @param body		Body of the requested announce.
 * @param group		GroupID64 of the group where the announce was requested.
 * @param errorCode Result error code if error, otherwise 0.
 * @param data		Extra data if any, otherwise 0
 */
forward void OnCommunityGroupAnnounceResult(const char[] title, const char[] body, const char[] group, int errorCode, any data);

/**
 * Called at the end of a group invite request from SteamCommunityGroupInvite().
 *
 * @param invitee	SteamID64 of the invited account.
 * @param group		GroupID64 of the group where the account was invited.
 * @param errorCode Result error code if error, otherwise 0.
 * @param data		Extra data if any, otherwise 0
 */
forward void OnCommunityGroupInviteResult(const char[] invitee, const char[] group, int errorCode, any data);

/**
 * Called at the end of a friend add request from SteamCommunityAddFriend().
 *
 * @param friend	SteamID64 of the added friend.
 * @param errorCode Result error code if error, otherwise 0.
 * @param data		Extra data if any, otherwise 0
 */
forward void OnCommunityAddFriendResult(const char[] friend, int errorCode, any data);

/**
 * Called at the end of a friend remove request from SteamCommunityRemoveFriend().
 *
 * @param friend	SteamID64 of the removed friend.
 * @param errorCode Result error code if error, otherwise 0.
 * @param data		Extra data if any, otherwise 0
 */
forward void OnCommunityRemoveFriendResult(const char[] friend, int errorCode, any data);

/**
 * CHAT FUNCTIONS
 *
 * - ONLY FULL STEAM ACCOUNTS ALLOWED.
 *
 * - After calling SteamChatConnect(), SteamCore will attempt to keep the chat connected most of the time.
 *
 * - While connected, SteamCore will be polling messages as soon as posible in a persistent connection.
 *
 * - If while connected, account is logged out, SteamCore will reconnect to chat as soon as it logs in.
 *
 * - If while connected, the connection times out, Steam servers are down or server is not connected to internet, 
 * Steamcore will attempt to reconnect every 30 seconds.
 *
 * - If there is a different error than the ones mentioned in the point above, SteamCore won't attempt to reconnect.
 *
 * - In the case of any error(timeout, http, etc.) while polling, OnChatDisconnected() will be called.
 *
 * - After a reconnect OnChatConnected() will be called.
 *
 * - If SteamChatConnect() or SteamChatDisconnect() is called one after the other, only the last one will have effect.
 */

native bool SteamChatConnect(SteamChatMode mode=SteamChatModeWEB);
native bool SteamChatDisconnect();
native bool SteamChatIsConnected();
native bool SteamChatSendMessage(const char[] friend, const char[] message, any data=0);

forward void OnChatConnected(int errorCode);
forward void OnChatDisconnected(int errorCode);
forward void OnChatMessageSent(const char[] friend, const char[] message, int errorCode, any data);
forward void OnChatMessageReceived(const char[] friend, const char[] message);

/**
 * Called when a friend changes their name or state.
 *
 * Notes:
 * - This is called for every friend who is already connected when SteamCore connects to chat.
 * - This is also called for self when SteamCore account changes state.
 * - This function can be used to check if an account accepted a friend request. Even if the added person is offline
 * they will trigger this function with 'state' == SteamChatStateOFFLINE.
 *
 * @param friend	SteamID64 of the friend who changed their name or state.
 * @param name		Friend's nickname.
 * @param state		Friend's state.
 */
forward void OnChatFriendStateChange(const char[] friend, const char[] name, SteamChatState state);
forward void OnChatRelationshipChange(const char[] account, SteamChatRelationship relationship);


/* HELPER STOCKS */

stock int GroupID64to32(const char[] GroupID64)
{
	if(strlen(GroupID64) < 10) return 0;

	char[] trimmedGroupID64[64];
	strcopy(trimmedGroupID64, sizeof(trimmedGroupID64), GroupID64[9]);
	return StringToInt(trimmedGroupID64) - 429521408;
}

// This function will stop working when Steam reaches 1 billion users. Currently 500 millions
stock int SteamID64to32(const char[] SteamID64)
{
	if(strlen(SteamID64) < 12) return -1;

	int ID32 = StringToInt(SteamID64[8]) - 960265728;
	if(ID32 <= 0) return ID32 + 1000000000;
	else return ID32;
}

// This function will stop working when Steam reaches 1 billion users. Currently 500 millions
stock void SteamID32to64(int SteamID32, char[] buffer, int maxlength)
{
	int ID64 = SteamID32 + 960265728;
	if(ID64 > 999999999) 
	{
		ID64 -= 1000000000;
		Format(buffer, maxlength, "76561198%09i", ID64);
	}
	else Format(buffer, maxlength, "76561197%09i", ID64);
}

stock int GetSteamID32fromClient(int client)
{
	char[] SteamID[32];
	GetClientAuthId(client, AuthId_Steam3, SteamID, sizeof SteamID);
	return StringToInt(SteamID[5]);
}

stock int FindClientFromSteamID32(int SteamID32)
{
	for(int i = 1; i <= MaxClients; i++) if(IsClientInGame(i) && !IsFakeClient(i) && SteamID32fromClient(i) == SteamID32) return i;
	return 0;
}

stock int FindClientFromSteamID64(const char[] SteamID64)
{
	char[] ID64[62];
	for(int i = 1; i <= MaxClients; i++)
	{
		if(IsClientInGame(i) && !IsFakeClient(i))
		{
			GetClientAuthId(i, AuthId_SteamID64, ID64, sizeof ID64);
			if(StrEqual(ID64, SteamID64)) return i;
		}
	}
	return 0;
}

public SharedPlugin __pl_steamcore = 
{
	name = "steamcore",
	file = "steamcore.smx",
#if defined REQUIRE_PLUGIN
	required = 1,
#else
	required = 0,
#endif
};