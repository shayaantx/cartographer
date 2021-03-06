
#include "Globals.h"

#include "XLive\IpManagement\XnIp.h"
#include "H2MOD\Modules\OnScreenDebug\OnscreenDebug.h"
#include "H2MOD\Modules\ServerConsole\ServerConsole.h"
#include "H2MOD\Modules\Config\Config.h"

#include "Networking.h"
#include "Memory\bitstream.h"
#include "CustomPackets\CustomPackets.h"
#include "NetworkConfiguration\NetworkConfiguration.h"

extern SOCKET game_network_data_gateway_socket_1000;
extern SOCKET game_network_message_gateway_socket_1001;

CustomNetwork *network = new CustomNetwork;

const char* getNetworkMessageName(int enumVal) {
	return network_message_name[enumVal];
}

void __cdecl request_write(void* a1, int a2, int a3) {
	bitstream::p_data_encode_integer()(a1, "identifier", *(DWORD *)a3, 32);
	bitstream::p_data_encode_integer()(a1, "flags", *(DWORD *)(a3 + 4), 8);
	LOG_TRACE_NETWORK("[H2MOD-network] connection request write, identifier={0}, flags={1}", *(DWORD *)a3, *(DWORD *)(a3 + 4));
}

bool __cdecl request_read(void* a1, int a2, int a3) {
	*(DWORD *)a3 = bitstream::p_data_decode_integer()(a1, "identifier", 32);
	*(DWORD *)(a3 + 4) = bitstream::p_data_decode_integer()(a1, "flags", 8);
	return bitstream::p_packet_is_valid()(a1) == 0;
}

void __cdecl refuse_write(void* a1, int a2, int a3) {
	bitstream::p_data_encode_integer()(a1, "remote-identifier", *(DWORD *)a3, 32);
	bitstream::p_data_encode_integer()(a1, "reason", *(DWORD *)(a3 + 4), 3);
}

bool __cdecl refuse_read(void* a1, int a2, int a3) {
	*(DWORD *)a3 = bitstream::p_data_decode_integer()(a1, "remote-identifier", 32);
	*(DWORD *)(a3 + 4) = bitstream::p_data_decode_integer()(a1, "reason", 3);
	bool isValid = bitstream::p_packet_is_valid()(a1) == 0;
	//LOG_TRACE_NETWORK_N("[H2MOD-network] connection refuse read, remote-identifier={}, reason=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
	return isValid;
}

void __cdecl establish_write(void* a1, int a2, int a3) {
	bitstream::p_data_encode_integer()(a1, "remote-identifier", *(DWORD *)a3, 32);
	bitstream::p_data_encode_integer()(a1, "reason", *(DWORD *)(a3 + 4), 32);
	//LOG_TRACE_NETWORK_N("[H2MOD-network] connection establish write, remote-identifier=%d, identifier=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4));
}

bool __cdecl establish_read(void* a1, int a2, int a3) {
	*(DWORD *)a3 = bitstream::p_data_decode_integer()(a1, "remote-identifier", 32);
	*(DWORD *)(a3 + 4) = bitstream::p_data_decode_integer()(a1, "identifier", 32);
	bool isValid = bitstream::p_packet_is_valid()(a1) == 0;
	//LOG_TRACE_NETWORK_N("[H2MOD-network] connection establish read, remote-identifier=%d, identifier=%d, isValid=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), isValid);
	return isValid;
}

void __cdecl closed_write(void* a1, int a2, int a3) {
	bitstream::p_data_encode_integer()(a1, "remote-identifier", *(DWORD *)a3, 32);
	bitstream::p_data_encode_integer()(a1, "identifier", *(DWORD *)(a3 + 4), 32);
	bitstream::p_data_encode_integer()(a1, "closure-reason", *(DWORD *)(a3 + 8), 5);
	//LOG_TRACE_NETWORK_N("[H2MOD-network] connection closed write, remote-identifier=%d, identifier=%d, closureReason=%d", *(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8));
}

bool __cdecl closed_read(void* a1, int a2, int a3) {
	bool result = false;
	signed int closure_reason; 

	*(DWORD *)a3 = bitstream::p_data_decode_integer()(a1, "remote-identifier", 32);
	*(DWORD *)(a3 + 4) = bitstream::p_data_decode_integer()(a1, "identifier", 32);
	*(DWORD *)(a3 + 8) = bitstream::p_data_decode_integer()(a1, "closure-reason", 5);

	if (bitstream::p_packet_is_valid()(a1))
		return result;
	
	closure_reason = *(DWORD *)(a3 + 8);
	if (closure_reason >= 0 && closure_reason < 18)
		result = true;
	
	//LOG_TRACE_NETWORK_N("[H2MOD-network] connection closed read, remote-identifier=%d, reason=%d, closureReason=%d, isValid=%d, result=%d",
	//	*(DWORD *)a3, *(DWORD *)(a3 + 4), *(DWORD *)(a3 + 8), isValid, result);
	return result;
}

bool __cdecl deserializePlayerAdd(void* a1, int a2, int a3) {
	typedef bool(__cdecl deserialize_player_add)(void* a1, int a2, int a3);
	auto p_deserialize_player_add = h2mod->GetAddress<deserialize_player_add*>(0x1F0752, 0x1D110B);

	return p_deserialize_player_add(a1, a2, a3);
}

typedef void(__cdecl *register_connection_packets)(void* a1);
register_connection_packets register_connection_packets_method;

void __cdecl registerConnectionPackets(void* packetObject) {
	register_packet_impl(packetObject, 4, "connect-request", 0, 8, 8, request_write, request_read, 0); //request
	register_packet_impl(packetObject, 5, "connect-refuse", 0, 8, 8, refuse_write, refuse_read, 0); //refuse
	register_packet_impl(packetObject, 6, "connect-establish", 0, 8, 8, establish_write, establish_read, 0); //establish
	register_packet_impl(packetObject, 7, "connect-closed", 0, 12, 12, closed_write, closed_read, 0); //closed
}

typedef void(__cdecl *register_player_packets)(void* a1);
register_player_packets register_player_packets_method;

void __cdecl registerPlayerPackets(void* packetObject) {

	register_packet_impl(packetObject, 25, "membership-update", 0, 15784, 15784,
		h2mod->GetAddress<void*>(0x1EF6B9, 0x1D0072),
		h2mod->GetAddress<void*>(0x1EFADD, 0x1D0496), 0);

	register_packet_impl(packetObject, 37, "virtual-couch-update", 0, 216, 216,
		h2mod->GetAddress<void*>(0x1F0042, 0x1D09FB),
		h2mod->GetAddress<void*>(0x1F0143, 0x1D0AFC), 0);

	register_packet_impl(packetObject, 38, "virtual-couch-request", 0, 208, 208,
		h2mod->GetAddress<void*>(0x1F0264, 0x1D0C1D),
		h2mod->GetAddress<void*>(0x1F031D, 0x1D0CD6), 0);

	register_packet_impl(packetObject, 26, "peer-properties", 0, 208, 208,
		h2mod->GetAddress<void*>(0x1F03F5, 0x1D0DAE),
		h2mod->GetAddress<void*>(0x1F04E0, 0x1D0E99), 0);

	register_packet_impl(packetObject, 27, "delegate-leadership", 0, 44, 44,
		h2mod->GetAddress<void*>(0x1F05EE, 0x1D0FA7),
		h2mod->GetAddress<void*>(0x1F061A, 0x1D0FD3), 0);

	register_packet_impl(packetObject, 28, "boot-machine", 0, 44, 44,
		h2mod->GetAddress<void*>(0x1F0652, 0x1D100B),
		h2mod->GetAddress<void*>(0x1F067E, 0x1D1037), 0);

	register_packet_impl(packetObject, 29, "player-add", 0, 168, 168,
		h2mod->GetAddress<void*>(0x1F06B6, 0x1D106F),
		(void*)deserializePlayerAdd, 0);

	register_packet_impl(packetObject, 30, "player-refuse", 0, 20, 20,
		h2mod->GetAddress<void*>(0x1F081F, 0x1D11D8),
		h2mod->GetAddress<void*>(0x1F085F, 0x1D1218), 0);

	register_packet_impl(packetObject, 31, "player-remove", 0, 12, 12,
		h2mod->GetAddress<void*>(0x1F08BC, 0x1D1275),
		h2mod->GetAddress<void*>(0x1F08EA, 0x1D12A3), 0);

	register_packet_impl(packetObject, 32, "player-properties", 0, 156, 156,
		h2mod->GetAddress<void*>(0x1F0935, 0x1D12EE),
		h2mod->GetAddress<void*>(0x1F09AC, 0x1D1365), 0);
}

bool decodePacketTypeAndSize(void *thisx, void* a2, signed int *a3, int a4) {
	char *v4; // ebp@1
	signed int v5; // eax@2
	int v6; // eax@4
	bool result; // al@7

	v4 = (char *)thisx;
	*a3 = bitstream::p_data_decode_integer()(a2, "type", 8);
	*(DWORD *)a4 = bitstream::p_data_decode_integer()(a2, "size", 16);
	LOG_TRACE_NETWORK("[h2mod-network] received packet decoded type={0}, typeName={1}, size={2}", *a3, getNetworkMessageName(*a3), *(DWORD *)a4);
	if (bitstream::p_packet_is_valid()(a2)
		|| (v5 = *a3, *a3 < 0)
		|| v5 >= 49
		|| (v6 = (int)&v4[32 * v5], !*(BYTE *)v6)
		|| *(DWORD *)a4 < *(DWORD *)(v6 + 12)
		|| *(DWORD *)a4 > *(DWORD *)(v6 + 16))
	{
		result = 0;
	}
	else
	{
		result = bitstream::p_packet_is_valid()(a2) == 0;
	}
	return result;
}

typedef char(__stdcall *receive_packet)(void *thisx, void* a2, int packetType, unsigned int *size, void *packet_obj);
receive_packet receive_packet_method;

char __stdcall receivePacket(void *thisx, void* a2, int packetType, unsigned int *size, void *packet_obj) {
	char *v5; // ebp@1
	int v6; // esi@2
	char result; // al@2
	LOG_TRACE_NETWORK("[h2mod-network] received packet");
	v5 = (char *)thisx;
	typedef bool(__thiscall* decode_type_and_size)(void* thisx, int a2, signed int* a3, int a4);
	decode_type_and_size decode_type_and_size_method = h2mod->GetAddress<decode_type_and_size>(0x1E8217, 0x1CA1DA);
	if (decodePacketTypeAndSize(thisx, a2, (signed int *)packetType, (int)size))
	{
		LOG_TRACE_NETWORK("[h2mod-network] received packet succesfully decoded");
		v6 = (int)&v5[32 * *(DWORD *)packetType];
		SecureZeroMemory(packet_obj, *size);
		result = (*(int(__cdecl **)(void*, unsigned int, void *))(v6 + 24))(a2, *size, packet_obj);// calls packet read/write method
	}
	else
	{
		LOG_TRACE_NETWORK("[h2mod-network] received packet was invalid");
		result = 0;
	}
	return result;
}

typedef void(__cdecl *serialize_parameters_update_packet)(void* a1, int a2, int a3);
serialize_parameters_update_packet serialize_parameters_update_packet_method;

void __cdecl serializeParametersUpdatePacket(void* a1, int a2, int a3) {
	serialize_parameters_update_packet_method(a1, a2, a3);
}

typedef bool(__stdcall *tjoin_game)(void* thisptr, int a2, int a3, XNKID* xnkid, XNKEY* xnkey, XNADDR* host_xn, int a7, int a8, int a9, int a10, int a11, char a12, int a13, int a14);
tjoin_game pjoin_game;

bool __stdcall join_game(void* thisptr, int a2, int a3, XNKID* xnkid, XNKEY* xnkey, XNADDR* host_xn, int a7, int a8, int a9, int a10, int a11, char a12, int a13, int a14)
{
	IN_ADDR ipIdentifier;

	memcpy(&ipManager.game_host_xn, host_xn, sizeof(XNADDR));
	LOG_TRACE_NETWORK("[H2MOD-Network] copied host information, XNADDR: {:#x}", ipManager.game_host_xn.ina.s_addr);
	memcpy(&ipManager.securePacket.xnkid, xnkid, sizeof(XNKID));
	XNetXnAddrToInAddr(host_xn, xnkid, &ipIdentifier);
	ipManager.SaveNatInfo(ipIdentifier, nullptr);
	ipManager.sendNatInfoUpdate(game_network_data_gateway_socket_1000, host_xn->wPortOnline);
	ipManager.sendNatInfoUpdate(game_network_message_gateway_socket_1001, ntohs(htons(ipManager.game_host_xn.wPortOnline) + 1));
	return pjoin_game(thisptr, a2, a3, xnkid, xnkey, host_xn, a7, a8, a9, a10, a11, a12, a13, a14);
}

typedef bool(__cdecl* decode_text_chat_packet_)(void* container, int a2, s_text_chat* data_structure);
decode_text_chat_packet_ p_decode_text_chat_packet;

bool __cdecl decode_text_chat_packet(void* container, int a2, s_text_chat* data_structure)
{
	bool ret = p_decode_text_chat_packet(container, a2, data_structure);

	return ret;
}

/* WIP */
/* All this does is patch some checks that cause using actual ip addresses not to work. */
/* When a call to XNetXnaddrtoInaddr happens we provide the actual ip address rather than a secure key */
typedef char(__stdcall *cmp_xnkid)(int thisx, int a2);
cmp_xnkid p_cmp_xnkid;

char __stdcall xnkid_cmp(int thisx, int a2) {
	return 1;
}

void removeXNetSecurity()
{
	DWORD dwBack;
	/* XNKEY bs */
	p_cmp_xnkid = (cmp_xnkid)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1C284A, 0x199F02), (BYTE*)xnkid_cmp, 9);
	VirtualProtect(p_cmp_xnkid, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	BYTE jmp = 0xEB;
	// apparently the secure address has 1 free byte 
	// after HTONL call, game is checking the al register (the lower 8 bits of eax register) if it is zero, if not everything network related will fail
	WriteBytes(h2mod->GetAddress(0x1B5DBE, 0x1961F8), &jmp, 1);
	NopFill(h2mod->GetAddress(0x1B624A, 0x196684), 2);
	NopFill(h2mod->GetAddress(0x1B6201, 0x19663B), 2);
	NopFill(h2mod->GetAddress(0x1B62BC, 0x1966F4), 2);
	
}

int __cdecl QoSLookUpImpl(int a1, signed int a2, int a3, int a4)
{
	return -1; // stub qos lookup function in-game between peers in a network session
}

void applyConnectionPatches()
{
	DWORD dwBack;
	//removeXNetSecurity();

	// live netcode research
	NetworkConfiguration::ApplyPatches();

	// stub QoS lookup function for in-game data
	PatchCall(h2mod->GetAddress(0x1BDCB0, 0x1B7B8A), QoSLookUpImpl);

	//NopFill<9>(h2mod->GetBase() + (h2mod->Server ? 0x1B3CC3 : 0x1F1F94)); // check if secure/ipaddress != 127.0.0.1
	//NopFill(h2mod->GetAddress() + (h2mod->Server ? 0x1B3CC3 : 0x1F1F94), 9); // check if secure/ipaddress != 127.0.0.1

	if (!h2mod->Server)
	{	
		pjoin_game = (tjoin_game)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1CDADE), (BYTE*)join_game, 13);
		VirtualProtect(pjoin_game, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	}
}

void CustomNetwork::applyNetworkHooks() {
	DWORD dwBack;
	DWORD serializeParametersUpdatePacketOffset = 0x1EDC41;

	///////////////////////////////////////////////
	//connection/player packet customizations below
	///////////////////////////////////////////////

	register_connection_packets_method = (register_connection_packets)DetourFunc(h2mod->GetAddress<BYTE*>(0x1F1B36, 0x1D24EF), (BYTE*)registerConnectionPackets, 5);
	VirtualProtect(register_connection_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	
	//use for debugging
	//register_player_packets_method = (register_player_packets)DetourFunc(h2mod->GetAddress<BYTE*>(0x1F0A55, 0x1D140E), (BYTE*)registerPlayerPackets, 5);
	//VirtualProtect(register_player_packets_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//serialize_parameters_update_packet_method = (serialize_parameters_update_packet)DetourFunc((BYTE*)h2mod->GetAddress(0x1F03F5, 0x1CE5FA), (BYTE*)serializeParametersUpdatePacket, 5);
	//VirtualProtect(serialize_parameters_update_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	/////////////////////////////////////////////////////////////////////
	//send/recv packet functions below (for troubleshooting and research)
	/////////////////////////////////////////////////////////////////////
	//send_packet_method = (send_packet)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1E8296, 0x1CA259), (BYTE*)sendPacket, 8);
	//VirtualProtect(send_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	//receive_packet_method = (receive_packet)DetourClassFunc(h2mod->GetAddress<BYTE*>(0x1E82E0, 0x1CA2A3), (BYTE*)receivePacket, 11);
	//VirtualProtect(receive_packet_method, 4, PAGE_EXECUTE_READWRITE, &dwBack);

	if (h2mod->Server) {
		p_decode_text_chat_packet = (decode_text_chat_packet_)DetourFunc(h2mod->GetAddress<BYTE*>(0x0, 0x1CD8A4), (BYTE*)decode_text_chat_packet, 12);
		VirtualProtect(p_decode_text_chat_packet, 4, PAGE_EXECUTE_READWRITE, &dwBack);
	}

	CustomPackets::ApplyGamePatches();
	applyConnectionPatches();
}