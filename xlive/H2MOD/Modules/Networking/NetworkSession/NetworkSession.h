#pragma once

#include "stdafx.h"
#include "..\Blam\Engine\Players\Players.h"

enum eNetwork_session_state : signed int
{
	network_session_state_none,
	network_session_state_peer_joining,
	network_session_state_peer_join_abort,
	network_session_state_peer_established,
	network_session_state_leaving,
	network_session_state_session_host,
	network_session_state_host_disband,
	network_session_state_host_handoff,
	network_session_state_host_reestablish,
	network_session_state_election,
	network_session_state_unk_2
};

enum eMap_status : int
{
	unk_map_stats,
	map_unavailable,
	unk_map_stats2,
	map_available,
	map_loaded,
	map_is_downloading
};

#pragma pack(push, 1)
struct network_address
{
	union
	{
		int ipv4;
		char ipv6[16];
	} address;
	short port;
	short address_type;
};

struct peer_information
{
	XNADDR address;
	BYTE gap_24[4];
	wchar_t name[16];
	wchar_t peer_session_name[32];
	eMap_status map_status;
	unsigned int map_progress_percentage;
	char field_70;
	BYTE gap_71[3];
	char field_74;
	BYTE gap_75[3];
	unsigned int nat_type;
	unsigned int connectivity_mask;
	unsigned int latency_min;
	unsigned int latency_est;
	unsigned int latency_max;
	BYTE gap_9C[68];
	DWORD field_F0;
	DWORD field_F4;
	DWORD gets_incremented_unk;
	signed int player_index[4]; // stores local players indexes of the peer (BIG TODO: maybe fix splitscreen at some point)
};
static_assert(sizeof(peer_information) == 268, "Invalid PeerNetworkInfo size");

struct peer_observer_channel
{
	bool field_0;
	bool field_1;
	char pad[2];
	signed int observer_index;
	int membership_update_number;
	int parameters_update_number;
	int virtual_couch_update_number;
	int vote_update_number;
	int field_18;
};

struct player_information
{
	XUID identifier; // -0xA
	DWORD peer_index; // -0x8
	DWORD peer_user_index; // -0x4
	WORD player_flags; // 0x0
	bool properties_valid;
	char pad[1];
	unsigned int controller_index;
	PlayerProperties properties;
	PlayerProperties player_properties_1;
	unsigned int player_voice;
	unsigned int player_text_chat;
};
static_assert(sizeof(player_information) == 296, "Invalid player_info size");

struct membership_info
{
	DWORD update_number; // 0x70
	DWORD session_leader_index; // 0x74
	XUID dedicated_server_xuid; // 0x7C
	DWORD field_80; // 0x80
	int peer_count; // 0x84
	peer_information peer_info[17]; // 0x88
	int player_count; // 0x1254
	DWORD players_active_mask; // 0x1258
	player_information player_info[16]; // 0x125C
	DWORD unk;
};
static_assert(sizeof(membership_info) == 9328, "Invalid membership_info size");

struct virtual_couch
{
	DWORD incremental_update_number;
	bool exists;
	XSESSION_INFO xsession_info;
	char pad[3];
	DWORD xuid_count;
	XUID xuid[16];
};
static_assert(sizeof(virtual_couch) == 200, "Invalid virtual_couch size");

struct session_parameters
{
	DWORD parameters_update_number;
	BYTE gap_4C64[4];
	int field_4C68;
	DWORD gap_4C6C;
	DWORD system_language_id;
	bool dedicated_server;
	char pad_1[3];
	DWORD party_privacy;
	int max_peers;
	signed int max_party_players;
	int gap_4C84;
	int field_4C88;
	int field_4C8C;
	DWORD field_4C90;
	DWORD field_4C94;
	BYTE gap_4C98[4];
	bool current_peer_is_host;
	char pad[3];
	DWORD field_4CA0;
	DWORD field_4CA4;
	BYTE gap_4CA8[16];
	DWORD field_4CB8;
	wchar_t scenario_path[128];
	BYTE gap4C00[12];
	BYTE random_bytes[8];
	DWORD dead_beef_lol;
	DWORD field_4DD4;
	DWORD field_4DD8;
	wchar_t custom_map_name[48];
	DWORD field_4E3C;
	BYTE gap_4E40[704];
	DWORD field_5100;
	BYTE gap_5104[3508];
	DWORD field_5EB8;
	BYTE gap_5EBC[76];
	DWORD field_5F08;
	DWORD field_5F0C;
};
static_assert(sizeof(session_parameters) == 4784, "Invalid session_params size");

struct network_session
{
	DWORD field_0;
	void *network_message_gateway_ptr;
	void *network_observer;
	DWORD session_manager_ptr;
	DWORD text_chat;
	int unk_index;
	int field_18;
	int network_protocol; // LIVE - 2, Network - 1
	XNKID session_id;
	wchar_t field_28[16];
	char field_48;
	XNKEY xnkey;
	char pad[3];
	int xnkey_index;
	signed int field_60;
	DWORD session_host_peer_index;
	int elected_host_peer_index;
	DWORD field_6C;
	membership_info membership; // 0x70
	membership_info membership_2; // ?? 
	virtual_couch v_couch;
	virtual_couch v_couch_2;
	char voting_information_1[160]; // unused
	char voting_information_2[160]; // unused
	BYTE gap_4B84[64];
	session_parameters parameters;
	session_parameters parameters_2;
	DWORD local_peer_index;
	peer_observer_channel peer_observer_channels[17];
	eNetwork_session_state local_session_state;
	DWORD time_unk_2;
	DWORD time_unk_3;
	DWORD time_unk;
	DWORD possible_new_peer_host_index;
	DWORD field_73B4;
	DWORD field_73B8;
	int field_73BC;
	char field_73C0[4];
	DWORD time_new_session_host_found;
	int field_73C8;
	DWORD field_73CC;
	int field_73D0;
	int field_73D4;
	int field_73D8;
	int field_73DC;
	BYTE field_73E0[32];
	int field_7400;
	int field_7404;
	BYTE gap_7408[4];
	int field_740C;
	int field_7410;
	BYTE gap_7414[100];
	DWORD field_7478;
	DWORD field_747C;
	DWORD field_7480;
	DWORD field_7484;
	BYTE gap_7488[1132];
	int field_78F4;
	int field_78F8;
	BYTE field_78FC[36];
	int field_7920;
	DWORD field_7924;
	DWORD total_join_request_attempts;
	DWORD last_time_join_request_sent;
	DWORD field_7930;
	DWORD field_7934;
	DWORD field_7938;
	signed int field_793C;
	signed int field_7940;
	signed int field_7944;
	signed int field_7948;
	signed int field_794C;
	signed int field_7950;
	signed int field_7954;
	signed int field_7958;
	signed int field_795C;
	DWORD field_7960;
	DWORD field_7964;
	DWORD field_7968;
	char field_796C[4];
	int field_7970;
	DWORD field_7974;
	DWORD field_7978;
	BYTE gap_797C[508];
	DWORD c_kablam_session_join_request_handler; // dedicated server session join handler
	char field_7B7C[12];
};
#pragma pack(pop)
static_assert(sizeof(network_session) == 31624, "Invalid network_session size");

namespace NetworkSession
{
	network_session* getNetworkSessions();
	network_session* getCurrentNetworkSession();
	bool getCurrentNetworkSession(network_session** a1);
	eNetwork_session_state getLocalSessionState();
	bool localPeerIsSessionHost();
	signed int getPeerIndexFromNetworkAddress(network_address* addr);
	char getMapFileLocation(wchar_t* buffer, size_t size);

	int getPeerCount();
	int getPeerIndexFromPlayerXuid(long long xuid);
	int getLocalPeerIndex();
	void kickPeer(int peerIndex);
	void logAllPeersToConsole();

	int getPlayerCount();
	bool IsActive(int playerIndex);
	player_information* getPlayerInformation(int playerIndex);
	int getPeerIndexFromPlayerIndex(int playerIndex);
	wchar_t* getPlayerName(int playerIndex);
	long long getPlayerXuidFromPlayerIndex(int playerIndex);
	int getPlayerTeamFromPlayerIndex(int playerIndex);
	int getPlayerTeamFromXuid(long long xuid);
	void logAllPlayersToConsole();
}

