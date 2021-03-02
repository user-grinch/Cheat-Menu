#include "pch.h"
#include "Vehicle.h"
#include "Menu.h"
#include "Ui.h"
#include "Util.h"

Vehicle::Vehicle()
{
	ParseVehiclesIDE();
	ParseCarcolsDAT();

	Events::processScriptsEvent += [this]
	{
		if (!images_loaded)
		{
			Util::LoadTexturesInDirRecursive(PLUGIN_PATH((char*)"CheatMenu\\vehicles\\images\\"), ".jpg", spawner::search_categories, spawner::image_vec);
			Util::LoadTexturesInDirRecursive(PLUGIN_PATH((char*)"CheatMenu\\vehicles\\components\\"), ".jpg", tune::search_categories, tune::image_vec);
			Util::LoadTexturesInDirRecursive(PLUGIN_PATH((char*)"CheatMenu\\vehicles\\paintjobs\\"), ".png", texture9::search_categories, texture9::image_vec);

			images_loaded = true;
		}

		uint timer = CTimer::m_snTimeInMilliseconds;
		CPlayerPed* player = FindPlayerPed();
		CVehicle* veh = player->m_pVehicle;

		if (player && veh)
		{
			int hveh = CPools::GetVehicleRef(veh);

			if (Ui::HotKeyPressed(Menu::hotkeys::flip_veh))
			{
				float roll;
				Command<Commands::GET_CAR_ROLL>(hveh, &roll);
				roll += 180;
				Command<Commands::SET_CAR_ROLL>(hveh, roll);
				Command<Commands::SET_CAR_ROLL>(hveh, roll); // z rot fix
			}

			if (Ui::HotKeyPressed(Menu::hotkeys::fix_veh))
			{
				player->m_pVehicle->Fix();
				player->m_pVehicle->m_fHealth = 1000.0f;
				CHud::SetHelpMessage("Vehicle fixed", false, false, false);
			}

			if (Ui::HotKeyPressed(Menu::hotkeys::veh_engine))
			{
				bool state = !veh->m_nVehicleFlags.bEngineBroken || veh->m_nVehicleFlags.bEngineOn;

				if (state)
					CHud::SetHelpMessage("Vehicle engine off", false, false, false);
				else
					CHud::SetHelpMessage("Vehicle engine on", false, false, false);

				veh->m_nVehicleFlags.bEngineBroken = state;
				veh->m_nVehicleFlags.bEngineOn = !state;
			}

			if (Ui::HotKeyPressed(Menu::hotkeys::veh_instant_start))
				Command<Commands::SET_CAR_FORWARD_SPEED>(hveh, 40.0f);

			if (Ui::HotKeyPressed(Menu::hotkeys::veh_instant_stop))
				Command<Commands::SET_CAR_FORWARD_SPEED>(hveh, 0);

			if (veh_nodmg)
			{
				veh->m_nPhysicalFlags.bBulletProof = true;
				veh->m_nPhysicalFlags.bExplosionProof = true;
				veh->m_nPhysicalFlags.bFireProof = true;
				veh->m_nPhysicalFlags.bCollisionProof = true;
				veh->m_nPhysicalFlags.bMeeleProof = true;
				veh->m_nVehicleFlags.bCanBeDamaged = true;
			}

			player->m_nPedFlags.CantBeKnockedOffBike = dont_fall_bike ? 1 : 2;
			Command<Commands::SET_CAR_HEAVY>(hveh, veh_heavy);
			Command<Commands::SET_CAR_WATERTIGHT>(hveh, veh_watertight);

			if (unlimited_nitro::enabled && player->m_pVehicle->m_nVehicleSubClass == VEHICLE_AUTOMOBILE)
			{
				patch::Set<BYTE>(0x969165, 0, true); // All cars have nitro
				patch::Set<BYTE>(0x96918B, 0, true); // All taxis have nitro

				if (KeyPressed(VK_LBUTTON))
				{
					if (!unlimited_nitro::comp_added)
					{
						AddComponent("1010", false);
						unlimited_nitro::comp_added = true;
					}
				}
				else
				{
					if (unlimited_nitro::comp_added)
					{
						RemoveComponent("1010", false);
						unlimited_nitro::comp_added = false;
					}
				}
			}

			if (lock_speed)
				Command<Commands::SET_CAR_FORWARD_SPEED>(hveh, lock_speed_val);

			if (neon::rainbow && timer - neon::rainbow_timer > 50)
			{
				int red, green, blue;

				Util::RainbowValues(red, green, blue, 0.25);
				InstallNeon(veh, red, green, blue);
				neon::rainbow_timer = timer;
			}
		}

		// Traffic neons
		if (neon::traffic && timer - neon::traffic_timer > 1000)
		{
			for (CVehicle* veh : CPools::ms_pVehiclePool)
			{
				int chance = 0;

				if (veh->m_nVehicleClass == CLASS_NORMAL) // Normal
					chance = Random(1, 20);

				if (veh->m_nVehicleClass == CLASS_RICHFAMILY) // Rich family
					chance = Random(1, 4);

				if (veh->m_nVehicleClass == CLASS_EXECUTIVE) // Executive
					chance = Random(1, 3);

				if (chance == 1 && !IsNeonInstalled(veh) && veh->m_pDriver != player)
					InstallNeon(veh, Random(0, 255), Random(0, 255), Random(0, 255));
			}
			neon::traffic_timer = timer;
		}

		if (bike_fly && veh && veh->IsDriver(player))
		{
			if (veh->m_nVehicleSubClass == VEHICLE_BIKE || veh->m_nVehicleSubClass == VEHICLE_BMX)
			{
				if (sqrt(veh->m_vecMoveSpeed.x * veh->m_vecMoveSpeed.x
					+ veh->m_vecMoveSpeed.y * veh->m_vecMoveSpeed.y
					+ veh->m_vecMoveSpeed.z * veh->m_vecMoveSpeed.z
				) > 0.0
				&& CTimer::ms_fTimeStep > 0.0)
				{
					veh->FlyingControl(3, -9999.9902f, -9999.9902f, -9999.9902f, -9999.9902f);
				}
			}
		}
	};
}

void Vehicle::AddComponent(const std::string& component, const bool display_message)
{
	try {
		CPlayerPed* player = FindPlayerPed();
		int icomp = std::stoi(component);
		int hveh = CPools::GetVehicleRef(player->m_pVehicle);

		CStreaming::RequestModel(icomp, eStreamingFlags::PRIORITY_REQUEST);
		CStreaming::LoadAllRequestedModels(true);
		player->m_pVehicle->AddVehicleUpgrade(icomp);
		CStreaming::SetModelIsDeletable(icomp);

		if (display_message)
			CHud::SetHelpMessage("Component added", false, false, false);
	}
	catch (...)
	{
		flog << "Failed to component to vehicle " << component << std::endl;
	}
}


void Vehicle::RemoveComponent(const std::string& component, const bool display_message)
{
	try {
		CPlayerPed* player = FindPlayerPed();
		int icomp = std::stoi(component);
		int hveh = CPools::GetVehicleRef(player->m_pVehicle);

		player->m_pVehicle->RemoveVehicleUpgrade(icomp);

		if (display_message)
			CHud::SetHelpMessage("Component removed", false, false, false);
	}
	catch (...)
	{
		flog << "Failed to remove component from vehicle " << component << std::endl;
	}
}

// Why did I do this shit? Guess it was the weather
int Vehicle::GetRandomTrainIdForModel(int model)
{
	static int train_ids[] = {
		8,9,			// model 449
		0,3,6,10,12,13, // model 537
		1,5,15			// model 538
	};
	int _start = 0, _end = 0;

	switch (model)
	{
		case 449:
			_start = 0;
			_end = 1;
			break;
		case 537:
			_start = 2;
			_end = 7;
			break;
		case 538:
			_start = 8;
			_end = 10;
			break;
		default:
			CHud::SetHelpMessage("Invalid train model", false, false, false);
			return -1;
	}
	int id = Random(_start, _end);
	return train_ids[id];
}

// Get vehicle HandlingId
void Vehicle::ParseVehiclesIDE()
{
	std::string file_path = std::string(paths::GetGameDirPathA()) + "/data/vehicles.ide";

	if (fs::exists(file_path))
	{
		std::ifstream file(file_path);
		std::string line;

		while (getline(file, line))
		{
			if (line[0] <= '0' || line[0] >= '9')
				continue;

			try
			{
				std::string temp;
				std::stringstream ss(line);

				// model
				getline(ss, temp, ',');

				int model = std::stoi(temp);

				// modelname, txd, type, handlingId
				getline(ss, temp, ',');
				getline(ss, temp, ',');
				getline(ss, temp, ',');
				getline(ss, temp, ',');

				temp.erase(std::remove_if(temp.begin(), temp.end(), ::isspace), temp.end());

				vehicle_ide[model] = temp;
			}
			catch (...)
			{
				flog << "Error while parsing line, " << line << std::endl;
			}
		}

		file.close();
	}
	else flog << "Vehicle.ide file not found";
}

void Vehicle::ParseCarcolsDAT()
{
	std::string file_path = std::string(paths::GetGameDirPathA()) + "/data/carcols.dat";

	if (fs::exists(file_path))
	{
		std::ifstream file(file_path);
		std::string line;

		bool car_section = false;
		bool col_section = false;
		int count = 0;
		while (getline(file, line))
		{
			if (line[0] == '#' || line == "")
				continue;

			if (line[0] == 'c' && line[1] == 'a' && line[2] == 'r')
			{
				car_section = true;
				continue;
			}

			if (line[0] == 'c' && line[1] == 'o' && line[2] == 'l')
			{
				col_section = true;
				continue;
			}

			if (line[0] == 'e' && line[1] == 'n' && line[2] == 'd')
			{
				car_section = false;
				col_section = false;
				continue;
			}

			if (col_section)
			{
				try
				{
					std::string temp;
					std::stringstream ss(line);

					std::replace(temp.begin(), temp.end(), '.', ','); // fix one instance where . is used instead of ,

					// red, green, blue
					getline(ss, temp, ',');
					int red = std::stoi(temp);

					getline(ss, temp, ',');
					int green = std::stoi(temp);

					getline(ss, temp, ',');
					int blue = std::stoi(temp);

					std::vector<float> color = { red / 255.0f,green / 255.0f,blue / 255.0f };
					carcols_color_values.push_back(color);

					++count;
				}
				catch (...)
				{
					flog << "Error while parsing car line, " << line << std::endl;
				}
			}

			if (car_section)
			{
				std::string temp;
				std::stringstream ss(line);

				getline(ss, temp, ',');
				std::string name = temp;
				while (getline(ss, temp, ','))
				{
					try
					{
						std::for_each(name.begin(), name.end(), [](char& c) {
							c = ::toupper(c);
						});

						int val = std::stoi(temp);
						if (!(std::find(carcols_car_data[name].begin(), carcols_car_data[name].end(), val) != carcols_car_data[name].end()))
							carcols_car_data[name].push_back(val);
					}
					catch (...)
					{
						flog << "Error while parsing car line, " << line << std::endl;
					}
				}
			}
		}

		file.close();
	}
	else flog << "Error locating Vehicle.ide";
}

void Vehicle::SpawnVehicle(std::string& smodel)
{
	CPlayerPed* player = FindPlayerPed();
	int hplayer = CPools::GetPedRef(player);

	int imodel = std::stoi(smodel);
	CVehicle* veh = nullptr;

	int interior = player->m_nAreaCode;

	if (Command<Commands::IS_MODEL_AVAILABLE>(imodel))
	{
		CVector pos = player->GetPosition();
		int speed = 0;

		if (player->m_nPedFlags.bInVehicle && spawner::spawn_inside)
		{
			int hveh = 0;
			Command<Commands::GET_CAR_CHAR_IS_USING>(hplayer, &hveh);
			CVehicle* pveh = CPools::GetVehicle(hveh);
			pos = pveh->GetPosition();

			Command<Commands::GET_CAR_SPEED>(hveh, &speed);

			Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(hplayer, pos.x, pos.y, pos.z);

			if (pveh->m_nVehicleClass == VEHICLE_TRAIN)
				Command<Commands::DELETE_MISSION_TRAIN>(hveh);
			else
				Command<Commands::DELETE_CAR>(hveh);
		}

		if (interior == 0)
			if (spawner::spawn_in_air && (CModelInfo::IsHeliModel(imodel) || CModelInfo::IsPlaneModel(imodel)))
				pos.z = 400;
			else
				pos.z -= 5;


		if (CModelInfo::IsTrainModel(imodel))
		{
			int train_id = GetRandomTrainIdForModel(imodel);

			if (train_id == -1) // Unknown train id
				return;

			int hveh = 0;

			// Loading all train related models
			CStreaming::RequestModel(590, PRIORITY_REQUEST);
			CStreaming::RequestModel(538, PRIORITY_REQUEST);
			CStreaming::RequestModel(570, PRIORITY_REQUEST);
			CStreaming::RequestModel(569, PRIORITY_REQUEST);
			CStreaming::RequestModel(537, PRIORITY_REQUEST);
			CStreaming::RequestModel(449, PRIORITY_REQUEST);

			CStreaming::LoadAllRequestedModels(false);

			CTrain* train = nullptr;
			CTrain* carraige = nullptr;
			int track = Random(0, 1);
			int node = CTrain::FindClosestTrackNode(pos, &track);
			CTrain::CreateMissionTrain(pos, (Random(0, 1)) == 1 ? true : false, train_id, &train, &carraige, node, track, false);

			veh = (CVehicle*)train;
			hveh = CPools::GetVehicleRef(veh);
			if (veh->m_pDriver)
				Command<Commands::DELETE_CHAR>(CPools::GetPedRef(veh->m_pDriver));

			if (spawner::spawn_inside)
			{
				Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, hveh);
				Command<Commands::SET_CAR_FORWARD_SPEED>(hveh, speed);
			}
			Command<Commands::MARK_MISSION_TRAIN_AS_NO_LONGER_NEEDED>(hveh);
			Command<Commands::MARK_CAR_AS_NO_LONGER_NEEDED>(hveh);
			CStreaming::SetModelIsDeletable(590);
			CStreaming::SetModelIsDeletable(538);
			CStreaming::SetModelIsDeletable(570);
			CStreaming::SetModelIsDeletable(569);
			CStreaming::SetModelIsDeletable(537);
			CStreaming::SetModelIsDeletable(449);
		}
		else
		{
			CStreaming::RequestModel(imodel, PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);

			if (spawner::license_text[0] != '\0')
				Command<Commands::CUSTOM_PLATE_FOR_NEXT_CAR>(imodel, spawner::license_text);

			int hveh = 0;
			if (spawner::spawn_inside)
			{
				Command<Commands::CREATE_CAR>(imodel, pos.x, pos.y, pos.z + 4.0f, &hveh);
				veh = CPools::GetVehicle(hveh);
				veh->SetHeading(player->GetHeading());
				Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, hveh);
				Command<Commands::SET_CAR_FORWARD_SPEED>(hveh, speed);
			}
			else
			{
				player->TransformFromObjectSpace(pos, CVector(0, 10, 0));

				Command<Commands::CREATE_CAR>(imodel, pos.x, pos.y, pos.z + 3.0f, &hveh);
				veh = CPools::GetVehicle(hveh);
				veh->SetHeading(player->GetHeading() + 55.0f);
			}
			veh->m_nDoorLock = CARLOCK_UNLOCKED;
			veh->m_nAreaCode = interior;
			Command<Commands::MARK_CAR_AS_NO_LONGER_NEEDED>(CPools::GetVehicleRef(veh));
			CStreaming::SetModelIsDeletable(imodel);
		}
		veh->m_nVehicleFlags.bHasBeenOwnedByPlayer = true;
	}
}

std::string Vehicle::GetNameFromModel(int model)
{
	CBaseModelInfo* info = CModelInfo::GetModelInfo(model);

	return (const char*)info + 0x32;
}

int Vehicle::GetModelFromName(const char* name)
{
	int model = 0;
	CBaseModelInfo* model_info = CModelInfo::GetModelInfo((char*)name, &model);

	if (model > 0 && model < 1000000 && GetNameFromModel(model) != "")
		return model;
	else
		return 0;
}

Vehicle::~Vehicle()
{
}


void Vehicle::GenerateHandlingDataFile(int phandling)
{
	FILE* fp = fopen("handling.txt", "w");

	std::string handlingId = vehicle_ide[FindPlayerPed()->m_pVehicle->m_nModelIndex];
	float fMass = patch::Get<float>(phandling + 0x4);
	float fTurnMass = patch::Get<float>(phandling + 0xC);
	float fDragMult = patch::Get<float>(phandling + 0x10);
	float CentreOfMassX = patch::Get<float>(phandling + 0x14);
	float CentreOfMassY = patch::Get<float>(phandling + 0x18);
	float CentreOfMassZ = patch::Get<float>(phandling + 0x1C);
	int nPercentSubmerged = patch::Get<int>(phandling + 0x20);
	float fTractionMultiplier = patch::Get<float>(phandling + 0x28);
	float fTractionLoss = patch::Get<float>(phandling + 0xA4);
	float TractionBias = patch::Get<float>(phandling + 0xA8);
	float fEngineAcceleration = patch::Get<float>(phandling + 0x7C) * 12500;
	float fEngineInertia = patch::Get<float>(phandling + 0x80);
	int nDriveType = patch::Get<BYTE>(phandling + 0x74);
	int nEngineType = patch::Get<BYTE>(phandling + 0x75);
	float BrakeDeceleration = patch::Get<float>(phandling + 0x94) * 2500;
	float BrakeBias = patch::Get<float>(phandling + 0x98);
	int ABS = patch::Get<BYTE>(phandling + 0x9C);
	float SteeringLock = patch::Get<float>(phandling + 0xA0);
	float SuspensionForceLevel = patch::Get<float>(phandling + 0xAC);
	float SuspensionDampingLevel = patch::Get<float>(phandling + 0xB0);
	float SuspensionHighSpdComDamp = patch::Get<float>(phandling + 0xB4);
	float Suspension_upper_limit = patch::Get<float>(phandling + 0xB8);
	float Suspension_lower_limit = patch::Get<float>(phandling + 0xBC);
	float Suspension_bias = patch::Get<float>(phandling + 0xC0);
	float Suspension_anti_dive_multiplier = patch::Get<float>(phandling + 0xC4);
	float fCollisionDamageMultiplier = patch::Get<float>(phandling + 0xC8) * 0.338;
	int nMonetaryValue = patch::Get<int>(phandling + 0xD8);

	int MaxVelocity = patch::Get<float>(phandling + 0x84);
	MaxVelocity = MaxVelocity * 206 + (MaxVelocity - 0.918668) * 1501;

	int modelFlags = patch::Get<int>(phandling + 0xCC);
	int handlingFlags = patch::Get<int>(phandling + 0xD0);

	int front_lights = patch::Get<BYTE>(phandling + 0xDC);
	int rear_lights = patch::Get<BYTE>(phandling + 0xDD);
	int vehicle_anim_group = patch::Get<BYTE>(phandling + 0xDE);
	int nNumberOfGears = patch::Get<BYTE>(phandling + 0x76);
	float fSeatOffsetDistance = patch::Get<float>(phandling + 0xD4);

	fprintf(fp, "\n%s\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%d\t%.5g\t%.5g\t%.5g\t%d\t%d\t%.5g\t%.5g\t%c\t%c\t%.5g\t%.5g\t%d\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g\t%d\t%d\t%d\t%d\t%d\t%d",
		handlingId.c_str(), fMass, fTurnMass, fDragMult, CentreOfMassX, CentreOfMassY, CentreOfMassZ, nPercentSubmerged, fTractionMultiplier, fTractionLoss, TractionBias, nNumberOfGears,
		MaxVelocity, fEngineAcceleration, fEngineInertia, nDriveType, nEngineType, BrakeDeceleration, BrakeBias, ABS, SteeringLock, SuspensionForceLevel, SuspensionDampingLevel,
		SuspensionHighSpdComDamp, Suspension_upper_limit, Suspension_lower_limit, Suspension_bias, Suspension_anti_dive_multiplier, fSeatOffsetDistance,
		fCollisionDamageMultiplier, nMonetaryValue, modelFlags, handlingFlags, front_lights, rear_lights, vehicle_anim_group);

	fclose(fp);
}


void Vehicle::Draw()
{
	ImGui::Spacing();
	static CPlayerPed* player = FindPlayerPed();
	static int hplayer = CPools::GetPedRef(player);

	if (ImGui::Button("Blow up cars", ImVec2(Ui::GetSize(3))))
		((void(*)(void))0x439D80)();

	ImGui::SameLine();

	if (ImGui::Button("Fix vehicle", ImVec2(Ui::GetSize(3))))
	{
		if (player && player->m_pVehicle)
		{
			player->m_pVehicle->Fix();
			player->m_pVehicle->m_fHealth = 1000.0f;
		}

	}

	ImGui::SameLine();

	if (ImGui::Button("Flip vehicle", ImVec2(Ui::GetSize(3))))
	{
		if (player->m_nPedFlags.bInVehicle)
		{
			int hveh = CPools::GetVehicleRef(player->m_pVehicle);
			float roll;

			Command<Commands::GET_CAR_ROLL>(hveh, &roll);
			roll += 180;
			Command<Commands::SET_CAR_ROLL>(hveh, roll);
			Command<Commands::SET_CAR_ROLL>(hveh, roll); // z rot fix
		}
	}

	ImGui::Spacing();

	if (ImGui::BeginTabBar("Vehicle", ImGuiTabBarFlags_NoTooltip + ImGuiTabBarFlags_FittingPolicyScroll))
	{
		CVehicle* pVeh = player->m_pVehicle;
		bool is_driver = pVeh && player->m_pVehicle->IsDriver(player);

		ImGui::Spacing();

		if (ImGui::BeginTabItem("Checkboxes"))
		{
			ImGui::Spacing();
			ImGui::BeginChild("CheckboxesChild");
			ImGui::Columns(2, 0, false);

			Ui::CheckboxAddress("Aggressive drivers", 0x96914F);
			Ui::CheckboxAddress("Aim while driving", 0x969179);
			Ui::CheckboxAddress("All cars have nitro", 0x969165);
			Ui::CheckboxAddress("All taxis have nitro", 0x96918B);
			Ui::CheckboxWithHint("Bikes fly", &bike_fly);
			Ui::CheckboxAddress("Boats fly", 0x969153);
			Ui::CheckboxAddress("Cars fly", 0x969160);
			Ui::CheckboxWithHint("Cars heavy", &veh_heavy);
			if (Ui::CheckboxWithHint("Damage proof", &veh_nodmg, "Every vehicle entered will be damage proof\nBullet, Collision, Explosion, Fire, Meele etc"))
			{
				if (pVeh && !veh_nodmg)
				{
					pVeh->m_nVehicleFlags.bCanBeDamaged = true;
					pVeh->m_nPhysicalFlags.bBulletProof = false;
					pVeh->m_nPhysicalFlags.bExplosionProof = false;
					pVeh->m_nPhysicalFlags.bFireProof = false;
					pVeh->m_nPhysicalFlags.bCollisionProof = false;
					pVeh->m_nPhysicalFlags.bMeeleProof = false;
				}
			}
			Ui::CheckboxAddress("Decreased traffic", 0x96917A);

			ImGui::NextColumn();

			Ui::CheckboxWithHint("Don't fall off bike", &dont_fall_bike);
			Ui::CheckboxAddress("Drive on water", 0x969152);
			Ui::CheckboxAddressEx("Lock train camera", 0x52A52F, 171, 6);
			Ui::CheckboxAddress("Float away when hit", 0x969166);
			Ui::CheckboxAddress("Green traffic lights", 0x96914E);
			Ui::CheckboxAddress("Perfect handling", 0x96914C);
			Ui::CheckboxAddress("Tank mode", 0x969164);
			Ui::CheckboxWithHint("Unlimited nitro", &unlimited_nitro::enabled, "Nitro will activate when left clicked\n\
\nEnabling this would disable\nAll cars have nitro\nAll taxis have nitro");
			Ui::CheckboxWithHint("Watertight car", &veh_watertight);
			Ui::CheckboxAddress("Wheels only", 0x96914B);
			ImGui::Columns(1);

			if (is_driver)
			{
				ImGui::NewLine();
				ImGui::TextWrapped("For current vehicle,");

				ImGui::Columns(2, 0, false);

				bool state = pVeh->m_nVehicleFlags.bAlwaysSkidMarks;
				if (Ui::CheckboxWithHint("Always skid marks", &state, nullptr))
					pVeh->m_nVehicleFlags.bAlwaysSkidMarks = state;

				state = pVeh->m_nPhysicalFlags.bBulletProof;
				if (Ui::CheckboxWithHint("Bullet proof", &state, nullptr, veh_nodmg))
					pVeh->m_nPhysicalFlags.bBulletProof = state;

				state = pVeh->m_nPhysicalFlags.bCollisionProof;
				if (Ui::CheckboxWithHint("Collision proof", &state, nullptr, veh_nodmg))
					pVeh->m_nPhysicalFlags.bCollisionProof = state;

				state = pVeh->m_nVehicleFlags.bDisableParticles;
				if (Ui::CheckboxWithHint("Disable particles", &state, nullptr))
					pVeh->m_nVehicleFlags.bDisableParticles = state;

				state = pVeh->m_nVehicleFlags.bVehicleCanBeTargetted;
				if (Ui::CheckboxWithHint("Driver targetable", &state, "Driver can be targeted"))
					pVeh->m_nVehicleFlags.bVehicleCanBeTargetted = state;

				state = !pVeh->m_nVehicleFlags.bEngineBroken || pVeh->m_nVehicleFlags.bEngineOn;
				if (Ui::CheckboxWithHint("Engine on", &state, nullptr, !is_driver))
				{
					pVeh->m_nVehicleFlags.bEngineBroken = !state;
					pVeh->m_nVehicleFlags.bEngineOn = state;
				}
				state = pVeh->m_nPhysicalFlags.bExplosionProof;
				if (Ui::CheckboxWithHint("Explosion proof", &state, nullptr, veh_nodmg))
					pVeh->m_nPhysicalFlags.bExplosionProof = state;

				state = pVeh->m_nPhysicalFlags.bFireProof;
				if (Ui::CheckboxWithHint("Fire proof", &state, nullptr, veh_nodmg))
					pVeh->m_nPhysicalFlags.bFireProof = state;

				ImGui::NextColumn();

				state = pVeh->m_nVehicleFlags.bVehicleCanBeTargettedByHS;
				if (Ui::CheckboxWithHint("HS targetable", &state, "Heat Seaker missile can target this"))
					pVeh->m_nVehicleFlags.bVehicleCanBeTargettedByHS = state;

				state = !pVeh->m_bIsVisible;
				if (Ui::CheckboxWithHint("Invisible car", &state, nullptr, !is_driver))
					pVeh->m_bIsVisible = !state;

				state = !pVeh->ms_forceVehicleLightsOff;
				if (Ui::CheckboxWithHint("Lights on", &state, nullptr, !is_driver))
					pVeh->ms_forceVehicleLightsOff = !state;

				state = pVeh->m_nDoorLock == CARLOCK_LOCKED_PLAYER_INSIDE;
				if (Ui::CheckboxWithHint("Lock doors", &state, nullptr, !is_driver))
				{
					if (state)
						pVeh->m_nDoorLock = CARLOCK_LOCKED_PLAYER_INSIDE;
					else
						pVeh->m_nDoorLock = CARLOCK_UNLOCKED;
				}

				state = pVeh->m_nPhysicalFlags.bMeeleProof;
				if (Ui::CheckboxWithHint("Melee proof", &state, nullptr, veh_nodmg))
					pVeh->m_nPhysicalFlags.bMeeleProof = state;

				state = pVeh->m_nVehicleFlags.bPetrolTankIsWeakPoint;
				if (Ui::CheckboxWithHint("Petrol tank blow", &state, "Vehicle will blow up if petrol tank is shot"))
					pVeh->m_nVehicleFlags.bPetrolTankIsWeakPoint = state;

				state = pVeh->m_nVehicleFlags.bSirenOrAlarm;
				if (Ui::CheckboxWithHint("Siren", &state))
					pVeh->m_nVehicleFlags.bSirenOrAlarm = state;

				state = pVeh->m_nVehicleFlags.bTakeLessDamage;
				if (Ui::CheckboxWithHint("Take less dmg", &state, nullptr))
					pVeh->m_nVehicleFlags.bTakeLessDamage = state;

				ImGui::Columns(1);
			}

			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Menus"))
		{
			ImGui::Spacing();
			ImGui::BeginChild("MenusChild");
			Ui::EditFloat("Density multiplier", 0x8A5B20, 0, 1, 10);
			if (ImGui::CollapsingHeader("Enter nearest vehicle as"))
			{
				CPlayerPed* player = FindPlayerPed();
				int hplayer = CPools::GetPedRef(player);
				CVehicle* veh = Util::GetClosestVehicle();

				if (veh)
				{
					int seats = veh->m_nMaxPassengers;

					ImGui::Spacing();
					ImGui::Columns(2, 0, false);

					ImGui::Text(GetNameFromModel(veh->m_nModelIndex).c_str());
					ImGui::NextColumn();
					ImGui::Text((std::string("Total seats: ") + std::to_string(seats + 1)).c_str());
					ImGui::Columns(1);

					ImGui::Spacing();
					if (ImGui::Button("Driver", ImVec2(Ui::GetSize(2))))
						Command<Commands::WARP_CHAR_INTO_CAR>(hplayer, veh);

					for (int i = 0; i < seats; ++i)
					{
						if (i % 2 != 1)
							ImGui::SameLine();

						if (ImGui::Button((std::string("Passenger ") + std::to_string(i + 1)).c_str(), ImVec2(Ui::GetSize(2))))
							Command<Commands::WARP_CHAR_INTO_CAR_AS_PASSENGER>(hplayer, veh, i);
					}
				}
				else
					ImGui::Text("No nearby vehicles");

				ImGui::Spacing();
				ImGui::Separator();
			}
			if (ImGui::CollapsingHeader("Remove vehicles in radius"))
			{
				ImGui::InputInt("Radius", &veh_remove_radius);
				ImGui::Spacing();
				if (ImGui::Button("Remove vehicles", Ui::GetSize(1)))
				{
					CPlayerPed* player = FindPlayerPed();
					for (CVehicle* veh : CPools::ms_pVehiclePool)
					{
						if (DistanceBetweenPoints(veh->GetPosition(), player->GetPosition()) < veh_remove_radius
						&& player->m_pVehicle != veh)
							Command<Commands::DELETE_CAR>(CPools::GetVehicleRef(veh));
					}
				}
				ImGui::Spacing();
				ImGui::Separator();
			}
			if (ImGui::CollapsingHeader("Traffic options"))
			{
				static std::vector<Ui::NamedMemory> color{ {"Black", 0x969151}, { "Pink",0x969150 } };
				static std::vector<Ui::NamedMemory> type{ {"Cheap", 0x96915E}, { "Country",0x96917B }, { "Fast",0x96915F } };

				Ui::RadioButtonAddress("Color", color);
				ImGui::Spacing();
				Ui::RadioButtonAddress("Type", type);
				ImGui::Spacing();
				ImGui::Separator();
			}

			if (player && player->m_pVehicle)
			{
				CVehicle* veh = player->m_pVehicle;
				int hveh = CPools::GetVehicleRef(veh);

				Ui::EditFloat("Dirt level", (int)veh + 0x4B0, 0, 7.5, 15);
				if (veh->m_nVehicleClass == VEHICLE_AUTOMOBILE && ImGui::CollapsingHeader("Doors"))
				{
					ImGui::Columns(2, 0, false);
					ImGui::RadioButton("Damage", &door_menu_button, 0);
					ImGui::RadioButton("Fix", &door_menu_button, 1);
					ImGui::NextColumn();
					ImGui::RadioButton("Open", &door_menu_button, 2);
					ImGui::RadioButton("Pop", &door_menu_button, 3);
					ImGui::Columns(1);
					ImGui::Spacing();

					int seats = veh->m_nMaxPassengers + 1; // passenger + driver
					int doors = seats == 4 ? 6 : 4;
					int hveh = CPools::GetVehicleRef(veh);

					if (ImGui::Button("All", ImVec2(Ui::GetSize())))
					{
						for (int i = 0; i < doors; ++i)
						{
							switch (door_menu_button)
							{
								case 0:
									Command<Commands::DAMAGE_CAR_DOOR>(hveh, i);
									break;
								case 1:
									Command<Commands::FIX_CAR_DOOR>(hveh, i);
									break;
								case 2:
									Command<Commands::OPEN_CAR_DOOR>(hveh, i);
									break;
								case 3:
									Command<Commands::POP_CAR_DOOR>(hveh, i);
									break;
								default:
									break;
							}
						}
					}

					for (int i = 0; i != doors; ++i)
					{
						if (ImGui::Button(door_names[i].c_str(), ImVec2(Ui::GetSize(2))))
						{
							switch (door_menu_button)
							{
								case 0:
									Command<Commands::DAMAGE_CAR_DOOR>(hveh, i);
									break;
								case 1:
									Command<Commands::FIX_CAR_DOOR>(hveh, i);
									break;
								case 2:
									Command<Commands::OPEN_CAR_DOOR>(hveh, i);
									break;
								case 3:
									Command<Commands::POP_CAR_DOOR>(hveh, i);
									break;
								default:
									break;
							}
						}

						if (i % 2 != 1)
							ImGui::SameLine();
					}

					ImGui::Spacing();
					ImGui::Separator();
				}

				if (ImGui::CollapsingHeader("Set speed"))
				{
					Ui::CheckboxWithHint("Lock speed", &lock_speed);
					ImGui::Spacing();
					ImGui::InputFloat("Set", &lock_speed_val);
					ImGui::Spacing();

					lock_speed_val = lock_speed_val > 100 ? 100 : lock_speed_val;
					lock_speed_val = lock_speed_val < 0 ? 0 : lock_speed_val;

					if (ImGui::Button("Set speed##brn", ImVec2(Ui::GetSize(2))))
						Command<Commands::SET_CAR_FORWARD_SPEED>(hveh, lock_speed_val);

					ImGui::SameLine();

					if (ImGui::Button("Instant stop##brn", ImVec2(Ui::GetSize(2))))
						Command<Commands::SET_CAR_FORWARD_SPEED>(hveh, 0);
				}
			}
			ImGui::EndChild();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Spawn"))
		{
			ImGui::Spacing();
			ImGui::Columns(2, 0, false);
			Ui::CheckboxWithHint("Spawn inside", &spawner::spawn_inside, "Spawn inside vehicle as driver");
			ImGui::NextColumn();
			Ui::CheckboxWithHint("Spawn aircraft in air", &spawner::spawn_in_air);
			ImGui::Columns(1);


			ImGui::Spacing();
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - 2.5);
			ImGui::InputTextWithHint("##LicenseText", "License plate text", spawner::license_text, 9);

			Ui::DrawImages(spawner::image_vec, ImVec2(100, 75), spawner::search_categories, spawner::selected_item, spawner::filter, SpawnVehicle, nullptr,
				[](std::string str)
			{
				return GetNameFromModel(std::stoi(str));
			});

			ImGui::EndTabItem();
		}
		if (player->m_pVehicle && player->m_nPedFlags.bInVehicle)
		{
			CVehicle* veh = FindPlayerPed()->m_pVehicle;
			int hveh = CPools::GetVehicleRef(veh);
			if (ImGui::BeginTabItem("Color"))
			{
				Paint::UpdateNodeListRecursive(veh);

				ImGui::Spacing();
				if (ImGui::Button("Reset color", ImVec2(Ui::GetSize())))
				{
					Paint::ResetNodeColor(veh, Paint::veh_nodes::selected);
					CHud::SetHelpMessage("Color reset", false, false, false);
				}
				ImGui::Spacing();

				Ui::ListBoxStr("Component", Paint::veh_nodes::names_vec, Paint::veh_nodes::selected);

				if (ImGui::ColorEdit3("Color picker", color::color_picker))
				{
					uchar r = color::color_picker[0] * 255;
					uchar g = color::color_picker[1] * 255;
					uchar b = color::color_picker[2] * 255;
					Paint::SetNodeColor(veh, Paint::veh_nodes::selected, { r,g,b,255 }, color::material_filter);
				}

				ImGui::Spacing();
				ImGui::Columns(2, NULL, false);
				ImGui::Checkbox("Material filter", &color::material_filter);
				ImGui::RadioButton("Primary", &color::radio_btn, 1);
				ImGui::RadioButton("Secondary", &color::radio_btn, 2);
				ImGui::NextColumn();
				ImGui::Checkbox("Show all", &color::show_all);
				ImGui::RadioButton("Tertiary", &color::radio_btn, 3);
				ImGui::RadioButton("Quaternary", &color::radio_btn, 4);
				ImGui::Spacing();
				ImGui::Columns(1);
				ImGui::Text("Select color preset:");
				ImGui::Spacing();

				int count = (int)carcols_color_values.size();

				ImVec2 size = Ui::GetSize();
				int btns_in_row = ImGui::GetWindowContentRegionWidth() / (size.y * 2);
				int btn_size = (ImGui::GetWindowContentRegionWidth() - int(ImGuiStyleVar_ItemSpacing) * (btns_in_row - 0.6 * btns_in_row)) / btns_in_row;

				ImGui::BeginChild("Colorss");

				if (color::show_all)
					for (int color_id = 0; color_id < count; ++color_id)
					{
						if (Ui::ColorButton(color_id, carcols_color_values[color_id], ImVec2(btn_size, btn_size)))
							*(uint8_replacement*)(int(veh) + 0x433 + color::radio_btn) = color_id;

						if ((color_id + 1) % btns_in_row != 0)
							ImGui::SameLine(0.0, 4.0);
					}
				else
				{
					std::string veh_name = GetNameFromModel(player->m_pVehicle->m_nModelIndex);
					for (auto entry : carcols_car_data)
					{
						if (entry.first == veh_name)
						{
							int count = 1;
							for (int color_id : entry.second)
							{
								if (Ui::ColorButton(color_id, carcols_color_values[color_id], ImVec2(btn_size, btn_size)))
									*(uint8_replacement*)(int(veh) + 0x433 + color::radio_btn) = color_id;

								if (count % btns_in_row != 0)
									ImGui::SameLine(0.0, 4.0);
								++count;
							}

							break;
						}
					}
				}

				ImGui::EndChild();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Neons"))
			{
				ImGui::Spacing();
				if (ImGui::Button("Remove neon", ImVec2(Ui::GetSize())))
				{
					RemoveNeon(veh);
					CHud::SetHelpMessage("Neon removed", false, false, false);
				}

				ImGui::Spacing();
				ImGui::Columns(2, NULL, false);

				bool pulsing = IsPulsingEnabled(veh);
				if (Ui::CheckboxWithHint("Pulsing neons", &pulsing))
					SetPulsing(veh, pulsing);

				Ui::CheckboxWithHint("Rainbow neons", &neon::rainbow, "Rainbow effect to neon lights");
				ImGui::NextColumn();
				Ui::CheckboxWithHint("Traffic neons", &neon::traffic, "Adds neon lights to traffic vehicles.\n\
Only some vehicles will have them.");
				ImGui::Columns(1);

				ImGui::Spacing();

				if (ImGui::ColorEdit3("Color picker", neon::color_picker))
					InstallNeon(veh, neon::color_picker[0] * 255, neon::color_picker[1] * 255, neon::color_picker[2] * 255);

				ImGui::Spacing();
				ImGui::Text("Select neon preset:");

				int count = (int)carcols_color_values.size();
				ImVec2 size = Ui::GetSize();
				int btns_in_row = ImGui::GetWindowContentRegionWidth() / (size.y * 2);
				int btn_size = (ImGui::GetWindowContentRegionWidth() - int(ImGuiStyleVar_ItemSpacing) * (btns_in_row - 0.6 * btns_in_row)) / btns_in_row;

				ImGui::BeginChild("Neonss");

				for (int color_id = 0; color_id < count; ++color_id)
				{
					if (Ui::ColorButton(color_id, carcols_color_values[color_id], ImVec2(btn_size, btn_size)))
					{
						std::vector<float>& color = carcols_color_values[color_id];
						InstallNeon(veh, color[0] * 255, color[1] * 255, color[2] * 255);
					}

					if ((color_id + 1) % btns_in_row != 0)
						ImGui::SameLine(0.0, 4.0);
				}

				ImGui::EndChild();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Textures"))
			{
				Paint::UpdateNodeListRecursive(veh);

				ImGui::Spacing();
				if (ImGui::Button("Reset texture", ImVec2(Ui::GetSize())))
				{
					Paint::ResetNodeTexture(veh, Paint::veh_nodes::selected);
					CHud::SetHelpMessage("Texture reset", false, false, false);
				}
				ImGui::Spacing();

				Ui::ListBoxStr("Component", Paint::veh_nodes::names_vec, Paint::veh_nodes::selected);
				ImGui::Spacing();

				int maxpjob, curpjob;
				Command<Commands::GET_NUM_AVAILABLE_PAINTJOBS>(hveh, &maxpjob);

				if (maxpjob > 0)
				{
					Command<Commands::GET_CURRENT_VEHICLE_PAINTJOB>(hveh, &curpjob);

					if (ImGui::InputInt("Paintjob", &curpjob))
					{
						if (curpjob > maxpjob)
							curpjob = -1;
						if (curpjob < -1)
							curpjob = maxpjob - 1;

						Command<Commands::GIVE_VEHICLE_PAINTJOB>(hveh, curpjob);
					}

					ImGui::Spacing();
				}

				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Checkbox("Material filter", &color::material_filter);
				ImGui::Spacing();
				Ui::DrawImages(texture9::image_vec, ImVec2(100, 80), texture9::search_categories, texture9::selected_item, texture9::filter,
					[](std::string& str)
					{
						Paint::SetNodeTexture(FindPlayerPed()->m_pVehicle, Paint::veh_nodes::selected, str, color::material_filter);
					},
					nullptr,
						[](std::string& str) {return str;
					});

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Tune"))
			{
				ImGui::Spacing();
				Ui::DrawImages(tune::image_vec, ImVec2(100, 80), tune::search_categories, tune::selected_item, tune::filter,
					[](std::string& str) {AddComponent(str);},
					[](std::string& str) {RemoveComponent(str); },
					[](std::string& str) {return str;},
					[](std::string& str)
					{
						return ((bool(*)(int, CVehicle*))0x49B010)(std::stoi(str), player->m_pVehicle);
					}
				);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Handling"))
			{
				ImGui::Spacing();

				CBaseModelInfo* info = CModelInfo::GetModelInfo(player->m_pVehicle->m_nModelIndex);
				int phandling = patch::Get<WORD>((int)info + 0x4A, false);
				phandling *= 0xE0;
				phandling += 0xC2B9DC;

				if (ImGui::Button("Reset handling", ImVec2(Ui::GetSize(3))))
				{
					gHandlingDataMgr.LoadHandlingData();
					CHud::SetHelpMessage("Handling reset", false, false, false);
				}

				ImGui::SameLine();

				if (ImGui::Button("Save to file", ImVec2(Ui::GetSize(3))))
				{
					GenerateHandlingDataFile(phandling);
					CHud::SetHelpMessage("Handling saved", false, false, false);
				}

				ImGui::SameLine();

				if (ImGui::Button("Read more", ImVec2(Ui::GetSize(3))))
					ShellExecute(NULL, "open", "https://projectcerbera.com/gta/sa/tutorials/handling", NULL, NULL, SW_SHOWNORMAL);

				ImGui::Spacing();

				ImGui::BeginChild("HandlingChild");

				static std::vector<Ui::NamedValue> abs{ { "On", 1 }, { "Off", 0 } };
				Ui::EditRadioButtonAddressEx("Abs", phandling + 0x9C, abs);

				Ui::EditFloat("Anti dive multiplier", phandling + 0xC4, 0.0f, 0.0f, 1.0f);
				Ui::EditFloat("Brake bias", phandling + 0x98, 0.0f, 0.0f, 1.0f);
				Ui::EditFloat("Brake deceleration", phandling + 0x94, 0.0f, 0.0f, 20.0f, 2500.0f);
				Ui::EditFloat("Centre of mass X", phandling + 0x14, -10.0f, -10.0f, 10.0f);
				Ui::EditFloat("Centre of mass Y", phandling + 0x18, -10.0f, -10.0f, 10.0f);
				Ui::EditFloat("Centre of mass Z", phandling + 0x1C, -10.0f, -10.0f, 10.0f);
				Ui::EditFloat("Collision damage multiplier", phandling + 0xC8, 0.0f, 0.0f, 1.0f, 0.3381f);
				Ui::EditFloat("Damping level", phandling + 0xB0, -10.0f, -10.0f, 10.0f); // test later
				Ui::EditFloat("Drag mult", phandling + 0x10, 0.0f, 0.0f, 30.0f);

				static std::vector<Ui::NamedValue> drive_type{ { "Front wheel drive", 70 }, { "Rear wheel drive", 82 }, { "Four wheel drive", 52 } };
				Ui::EditRadioButtonAddressEx("Drive type", phandling + 0x74, drive_type);

				Ui::EditFloat("Engine acceleration", phandling + 0x7C, 0.0f, 0.0f, 49.0f, 12500.0f);
				Ui::EditFloat("Engine inertia", phandling + 0x80, 0.0f, 0.0f, 400.0f);

				static std::vector<Ui::NamedValue> engine_type{ { "Petrol", 80 }, { "Diseal", 68 }, { "Electric", 69 } };
				Ui::EditRadioButtonAddressEx("Engine type", phandling + 0x75, engine_type);

				std::vector<Ui::NamedValue> front_lights{ { "Long", 0 }, { "Small", 1 }, { "Big", 2 }, { "Tall", 3 } };
				Ui::EditRadioButtonAddressEx("Front lights", phandling + 0xDC, front_lights);

				Ui::EditFloat("Force level", phandling + 0xAC, -10.0f, -10.0f, 10.0f); // test later

				Ui::EditBits("Handling flags", phandling + 0xD0, handling_flag_names);

				Ui::EditFloat("High speed damping", phandling + 0xB4, -10.0f, -10.0f, 10.0f);// test later
				Ui::EditFloat("Lower limit", phandling + 0xBC, -10.0f, -10.0f, 10.0f);// test later
				Ui::EditFloat("Mass", phandling + 0x4, 1.0f, 1.0f, 50000.0f);

				///fcommon.UpdateAddress({ name = 'Max velocity',address = phandling + 0x84 ,size = 4,min = 0,max = 2,is_float = true,cvalue = 0.01 , save = false })

				Ui::EditBits("Model flags", phandling + 0xCC, model_flag_names);

				Ui::EditAddress<int>("Monetary value", phandling + 0xD8, 1, 1, 100000);
				Ui::EditAddress<BYTE>("Number of gears", phandling + 0x76, 1, 1, 10);
				Ui::EditAddress<BYTE>("Percent submerged", phandling + 0x20, 10, 10, 120);

				static std::vector<Ui::NamedValue> rear_lights{ { "Long", 0 }, { "Small", 1 }, { "Big", 2 }, { "Tall", 3 } };
				Ui::EditRadioButtonAddressEx("Rear lights", phandling + 0xDD, rear_lights);

				Ui::EditFloat("Seat offset distance", phandling + 0xD4, 0.0f, 0.0f, 1.0f);
				Ui::EditFloat("Steering lock", phandling + 0xA0, 10.0f, 10.0f, 50.0f);
				Ui::EditFloat("Suspension bias", phandling + 0xC0, 0.0f, 0.0f, 1.0f);
				Ui::EditFloat("Traction bias", phandling + 0xA8, 0.0f, 0.0f, 1.0f);
				Ui::EditFloat("Traction loss", phandling + 0xA4, 0.0f, 0.0f, 1.0f);
				Ui::EditFloat("Traction multiplier", phandling + 0x28, 0.5f, 0.5f, 2.0f);
				Ui::EditFloat("Turn mass", phandling + 0xC, 20.0f, 20.0f, 1000.0f); // test later
				Ui::EditFloat("Upper limit", phandling + 0xB8, -1.0f, -1.0f, 1.0f);
				Ui::EditAddress<BYTE>("Vehicle anim group", phandling + 0xDE, 0, 0, 20);

				ImGui::EndChild();

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}
