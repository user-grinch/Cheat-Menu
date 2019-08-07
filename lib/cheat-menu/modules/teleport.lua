-- This module contains all the functions related to teleportation tab

local module = {}

-- Teleport table
local tteleport =
{
    shortcut       = imgui.new.bool(fconfig.get('tteleport.shortcut',false)),
    coords         = imgui.new.char[24](fconfig.get('tteleport.coords',"")),
    auto_z         = imgui.new.bool(fconfig.get('tteleport.auto_z',false)),
    insert_coords  = imgui.new.bool(fconfig.get('tteleport.insert_coords',false)),
    search_text    = imgui.new.char[64](fconfig.get('tteleport.search_text',"")),
}

module.tteleport = tteleport


local coordinates =
{
	["Interior: 24/7 1"]           						= "17, -25.884498, -185.868988, 1003.546875",
	["Interior: 24/7 2"]           						= "10, 6.091179, -29.271898, 1003.549438",
	["Interior: 24/7 3"]           						= "18, -30.946699, -89.609596, 1003.546875",
	["Interior: 24/7 4"]           						= "16, -25.132598, -139.066986, 1003.546875",
	["Interior: 24/7 5"]           						= "4, -27.312299, -29.277599, 1003.557250",
	["Interior: 24/7 6"]           						= "6, -26.691598, -55.714897, 1003.546875",
	["Interior: Airport ticket desk"]   				= "14, -1827.147338, 7.207417, 1061.143554",
	["Interior: Airport baggage reclaim"]				= "14, -1861.936889, 54.908092, 1061.143554",
	["Interior: Shamal"]           						= "1, 1.808619, 32.384357, 1199.593750",
	["Interior: Andromada"]           					= "9, 315.745086, 984.969299, 1958.919067",
	["Interior: Ammunation 1"]          				= "1, 286.148986, -40.644397, 1001.515625",
	["Interior: Ammunation 2"]          				= "4, 286.800994, -82.547599, 1001.515625",
	["Interior: Ammunation 3"]          				= "6, 296.919982, -108.071998, 1001.515625",
	["Interior: Ammunation 4"]          				= "7, 314.820983, -141.431991, 999.601562",
	["Interior: Ammunation 5"]          				= "6, 316.524993, -167.706985, 999.593750",
	["Interior: Ammunation booths"]     				= "7, 302.292877, -143.139099, 1004.062500",
	["Interior: Ammunation range"]      				= "7, 298.507934, -141.647048, 1004.054748",
	["Interior: Blastin fools hallway"] 				= "3, 1038.531372, 0.111030, 1001.284484",
	["Interior: Budget inn motel room"] 				= "12, 444.646911, 508.239044, 1001.419494",
	["Interior: Jefferson motel"]       				= "15, 2215.454833, -1147.475585, 1025.796875",
	["Interior: Off track betting shop"]				= "3, 833.269775, 10.588416, 1004.179687",
	["Interior: Sex shop"]      						= "3, -103.559165, -24.225606, 1000.718750",
	["Interior: Meat factory"]      					= "1, 963.418762, 2108.292480, 1011.030273",
	["Interior: Zeros RC shop"]      					= "6, -2240.468505, 137.060440, 1035.414062",
	["Interior: Dillimore gas station"] 				= "0, 663.836242, -575.605407, 16.343263",
	["Interior: Catigulas basement"]    				= "1, 2169.461181, 1618.798339, 999.976562",
	["Interior: FDC Janitors room"]     				= "10, 1889.953369, 1017.438293, 31.882812",
	["Interior: Woozies office"]        				= "1, -2159.122802, 641.517517, 1052.381713",
	["Interior: Binco"]      							= "15, 207.737991, -109.019996, 1005.132812",
	["Interior: Didier sachs"]          				= "14, 204.332992, -166.694992, 1000.523437",
	["Interior: Prolaps"] 	     						= "3, 207.054992, -138.804992, 1003.507812",
	["Interior: Suburban"]      						= "1, 203.777999, -48.492397, 1001.804687",
	["Interior: Victim"]      							= "5, 226.293991, -7.431529, 1002.210937",
	["Interior: Zip"]      								= "18, 161.391006, -93.159156, 1001.804687",
	["Interior: Club"]      							= "17, 493.390991, -22.722799, 1000.679687",
	["Interior: Bar"]      								= "11, 501.980987, -69.150199, 998.757812",
	["Interior: Lil probe inn"]      					= "18, -227.027999, 1401.229980, 27.765625",
	["Interior: Jays diner"]      						= "4, 457.304748, -88.428497, 999.554687",
	["Interior: Gant bridge diner"]     				= "5, 454.973937, -110.104995, 1000.077209",
	["Interior: Secret valley diner"]   				= "6, 435.271331, -80.958938, 999.554687",
	["Interior: World of coq"]      					= "1, 452.489990, -18.179698, 1001.132812",
	["Interior: Welcome pump"]      					= "1, 681.557861, -455.680053, -25.609874",
	["Interior: Burger shot"]      						= "10, 375.962463, -65.816848, 1001.507812",
	["Interior: Cluckin bell"]      					= "9, 369.579528, -4.487294, 1001.858886",
	["Interior: Well stacked pizza"]    				= "5, 373.825653, -117.270904, 1001.499511",
	["Interior: Rusty browns donuts"]   				= "17, 381.169189, -188.803024, 1000.632812",
	["Interior: Denise room"]      						= "1, 244.411987, 305.032989, 999.148437",
	["Interior: Katie room"]      						= "2, 271.884979, 306.631988, 999.148437",
	["Interior: Helena room"]      						= "3, 291.282989, 310.031982, 999.148437",
	["Interior: Michelle room"]      					= "4, 302.180999, 300.722991, 999.148437",
	["Interior: Barbara room"]      					= "5, 322.197998, 302.497985, 999.148437",
	["Interior: Millie room"]      						= "6, 346.870025, 309.259033, 999.155700",
	["Interior: Sherman dam"]      						= "17, -959.564392, 1848.576782, 9.000000",
	["Interior: Planning dept"]      					= "3, 384.808624, 173.804992, 1008.382812",
	["Interior: Area 51"]      							= "0, 223.431976, 1872.400268, 13.734375",
	["Interior: LS gym"]      							= "5, 772.111999, -3.898649, 1000.728820",
	["Interior: SF gym"]      							= "6, 774.213989, -48.924297, 1000.585937",
	["Interior: LV gym"]      							= "7, 773.579956, -77.096694, 1000.655029",
	["Interior: B Dups house"]      					= "3, 1527.229980, -11.574499, 1002.097106",
	["Interior: B Dups crack pad"]      				= "2, 1523.509887, -47.821197, 1002.130981",
	["Interior: Cj's house"]      						= "3, 2496.049804, -1695.238159, 1014.742187",
	["Interior: Madd Doggs mansion"]    				= "5, 1267.663208, -781.323242, 1091.906250",
	["Interior: Og Locs house"]      					= "3, 513.882507, -11.269994, 1001.565307",
	["Interior: Ryders house"]      					= "2, 2454.717041, -1700.871582, 1013.515197",
	["Interior: Sweets house"]      					= "1, 2527.654052, -1679.388305, 1015.498596",
	["Interior: Crack factory"]      					= "2, 2543.462646, -1308.379882, 1026.728393",
	["Interior: Big spread ranch"]      				= "3, 1212.019897, -28.663099, 1000.953125",
	["Interior: Fanny batters"]      					= "6, 761.412963, 1440.191650, 1102.703125",
	["Interior: Strip club"]      						= "2, 1204.809936, -11.586799, 1000.921875",
	["Interior: Strip club private room"]				= "2, 1204.809936, 13.897239, 1000.921875",
	["Interior: Unnamed brothel"]                       = "3, 942.171997, -16.542755, 1000.929687",
	["Interior: Tiger skin brothel"]                    = "3, 964.106994, -53.205497, 1001.124572",
	["Interior: Pleasure domes"]                        = "3, -2640.762939, 1406.682006, 906.460937",
	["Interior: Liberty city outside"]                  = "1, -729.276000, 503.086944, 1371.971801",
	["Interior: Liberty city inside"]                   = "1, -794.806396, 497.738037, 1376.195312",
	["Interior: Gang house"]      		                = "5, 2350.339843, -1181.649902, 1027.976562",
	["Interior: Colonel Furhberger's"]                  = "8, 2807.619873, -1171.899902, 1025.570312",
	["Interior: Crack den"]      		                = "5, 318.564971, 1118.209960, 1083.882812",
	["Interior: Warehouse 1"]      		                = "1, 1412.639892, -1.787510, 1000.924377",
	["Interior: Warehouse 2"]      		                = "18, 1302.519897, -1.787510, 1001.028259",
	["Interior: Sweets garage"]      	                = "0, 2522.000000, -1673.383911, 14.866223",
	["Interior: Lil probe inn toilet"]                  = "18, -221.059051, 1408.984008, 27.773437",
	["Interior: Unused safe house"]                     = "12, 2324.419921, -1145.568359, 1050.710083",
	["Interior: RC Battlefield"]                        = "10, -975.975708, 1060.983032, 1345.671875",
	["Interior: Barber 1"]      		                = "2, 411.625976, -21.433298, 1001.804687",
	["Interior: Barber 2"]      		                = "3, 418.652984, -82.639793, 1001.804687",
	["Interior: Barber 3"]      		                = "12, 412.021972, -52.649898, 1001.898437",
	["Interior: Tatoo parlour 1"]                       = "16, -204.439987, -26.453998, 1002.273437",
	["Interior: Tatoo parlour 2"]                       = "17, -204.439987, -8.469599, 1002.273437",
	["Interior: Tatoo parlour 3"]                       = "3, -204.439987, -43.652496, 1002.273437",
	["Interior: LS police HQ"]      	                = "6, 246.783996, 63.900199, 1003.640625",
	["Interior: SF police HQ"]      	                = "10, 246.375991, 109.245994, 1003.218750",
	["Interior: LV police HQ"]      	                = "3, 288.745971, 169.350997, 1007.171875",
	["Interior: Car school"]      		                = "3, -2029.798339, -106.675910, 1035.171875",
	["Interior: 8-Track"]      			                = "7, -1398.065307, -217.028900, 1051.115844",
	["Interior: Bloodbowl"]      		                = "15, -1398.103515, 937.631164, 1036.479125",
	["Interior: Dirt track"]      		                = "4, -1444.645507, -664.526000, 1053.572998",
	["Interior: Kickstart"]      		                = "14, -1465.268676, 1557.868286, 1052.531250",
	["Interior: Vice stadium"]      	                = "1, -1401.829956, 107.051300, 1032.273437",
	["Interior: SF Garage"]      		                = "0, -1790.378295, 1436.949829, 7.187500",
	["Interior: LS Garage"]      		                = "0, 1643.839843, -1514.819580, 13.566620",
	["Interior: SF Bomb shop"]      	                = "0, -1685.636474, 1035.476196, 45.210937",
	["Interior: Blueberry warehouse"]                   = "0, 76.632553, -301.156829, 1.578125",
	["Interior: LV Warehouse 1"]                        = "0, 1059.895996, 2081.685791, 10.820312",
	["Interior: LV Warehouse 2"]      	                = "0, 1059.180175, 2148.938720, 10.820312",
	["Interior: Catigulas"]      		                = "1, 2131.507812, 1600.818481, 1008.359375",
	["Interior: Bank"]      			                = "0, 2315.952880, -1.618174, 26.742187",
	["Interior: Bank"]      			                = "0, 2319.714843, -14.838361, 26.749565",
	["Interior: LS Atruim"]      		                = "18, 1710.433715, -1669.379272, 20.225049",
	["Los Santos : 8 Ball's Autos"]                     = "0, 1834.70385742188, -1856.40185546875, 13.3897476196289",
	["Los Santos : Airport"]                            = "0, 1682.73156738281, -2264.05786132813, 13.5079345703125",
	["Los Santos : Alhambra Club"]                      = "0, 1828.20690917969, -1682.60571289063, 13.546875",
	["Los Santos : Atrium"]                             = "0, 1722.0576171875, -1605.82824707031, 13.546875",
	["Los Santos : Big Smoke's Crack Palace"]           = "0, 2518.2197265625, -1272.61791992188, 34.8740921020508",
	["Los Santos : Big Smoke's Crack Palace (inside)"]  = "2, 2580.02124023438, -1300.33801269531, 1060.9921875",
	["Los Santos : Candy Suxx Billboard"]               = "0, 419.148376464844, -1368.41320800781, 41.140625X",
	["Los Santos : Cathay Theater"]                     = "0, 1022.50787353516, -1133.8857421875, 23.828125",
	["Los Santos : Cemetery"]                           = "0, 832.375915527344, -1101.96203613281, 24.296875",
	["Los Santos : CJ's home"]                          = "0, 2495.32275390625, -1685.42651367188, 13.5127220153809",
	["Los Santos : Colonel Fuhrberger's"]               = "0, 2820.60961914063, -1182.16235351563, 25.232967376709",
	["Los Santos : Didier Sachs Shop"]                  = "0, 450.887725830078, -1479.1318359375, 30.8057765960693",
	["Los Santos : Ghost Graffiti (20:00-6:00)"]        = "0, 927.000305175781, -1058.16821289063, 24.7002906799316",
	["Los Santos : Grotti (Ferrari) Billboard"]         = "0, 487.795166015625, -1326.62707519531, 28.0078125",
	["Los Santos : GTA III and GTA: Vice City logos"]   = "0, 788.071899414063, -1035.47729492188, 24.6282062530518",
	["Los Santos : Highest building of Los Santos"]     = "0, 1531.17175292969, -1357.61804199219, 329.453460693359",
	["Los Santos : Inside Track Betting"]               = "0, 1631.77648925781, -1167.0690979688, 24.078125",
	["Los Santos : Jefferson Hotel"]                    = "0, 2221.89038085938, -1159.849609375, 25.7330799102783",
	["Los Santos : Jesus Saves"]                        = "0, 2719.21704101563, -2009.17846679688, 13.5546875",
	["Los Santos : Lemmings Pole"]     		    	    = "0, 2392.33569335938, -1511.87316894531, 23.8348655700684",
	["Los Santos : Lowrider Competition"]               = "0, 1816.08728027344, -1929.91040039063, 13.5433483123779",
	["Los Santos : Market Station"]     		    	= "0, 826.761840820313, -1358.68713378906, -0.5078125",
	["Los Santos : Observatory"]     	    	    	= "0, 1191.98645019531, -2036.88049316406, 69.0078125",
	["Los Santos : Ocean Docks"]     		    		= "0, 2184.40185546875, -2260.38232421875, 13.4100790023804",
	["Los Santos : Police brutality"]     		    	= "0, 1540.00646972656, -1627.77075195313, 13.3828125",
	["Los Santos : Police Departament"]     	    	= "0, 1540.84509277344, -1675.45324707031, 13.5513896942139",
	["Los Santos : Roboi's Food Mart"]     		    	= "0, 1350.62097167969, -1744.802734375, 13.3808469772339",
	["Los Santos : Stadium"]     			    		= "0, 2684.78198242188, -1689.56359863281, 9.43039894104004",
	["Los Santos : Ten Green Bottles"]    				= "0, 2304.39965820313, -1651.30908203125, 14.5202884674072",
	["Los Santos : The Pig Pen Strip Club"]     	    = "0, 2421.12280273438, -1228.55883789063, 24.8796482086182",
	["Los Santos : Tiki Theater"]     		  		    = "0, 2348.78637695313, -1463.54272460938, 24",
	["Los Santos : Triathalon"]     				    = "0, 181.06819152832, -1866.71862792969, 3.04375839233398",
	["Los Santos : Verona Beach Gym"]     			    = "0, 666.700561523438, -1880.67272949219, 5.46000003814697",
	["Los Santos : Vinewood Sign"]     				    = "0, 1386.48901367188, -822.793640136719, 73.6923141479492",
	["Los Santos : Vice City Hidden Package"]     	    = "0, 2354.03198242188, -1467.3642578125, 31.9559421539307",
	["Palomino Creek : Bank"]     			  		    = "0, 2306.333984375, -15.3912973403931, 26.7495651245117",
	["Palomino Creek : Cementry"]     				    = "0, 2240.06396484375, -61.6567649841309, 26.50661277771",
	["Palomino Creek : Empty Diner"]     			    = "0, 2337.83618164063, 6.2173113822937, 26.484375",
	["Palomino Creek : Glowing headstones"]     	    = "0, 2240.09130859375, -70.5344467163086, 26.5068187713623",
	["Palomino Creek : Rusty Wheelchair"]     		    = "0, 2123.24487304688, -90.2263793945313, 2.16398048400879",
	["Palomino Creek : Triathalon"]     			    = "0, 2139.28759765625, -65.8627624511719, 2.96075963973999",
	["Montgomery : Biowaste Well"]     			   	    = "0, 1277.96606445313, 292.559906005859, 19.5546875",
	["Montgomery : Inside Track Betting"]    		    = "0, 1292.03247070313, 269.481689453125, 19.5546875",
	["Dillimore : Max Pane - Bulletproof Glass"]        = "0, 658.372924804688, -569.527587890625, 16.3359375",
	["Dillimore : Police Departament"]     		  	    = "0, 635.456298828125, -571.612915039063, 16.3359375",
	["Blueberry : Liquor Store"]    		    		= "0, 254.497406005859, -66.0255889892578, 1.578125",
	["Blueberry : Locals Only Shop"]    		    	= "0, 276.548187255859, -157.536209106445, 1.5704517364502",
	["Fern Ridge : Catalina's Ex-boyfriends"]    	    = "0, 843.333190917969, -23.9582138061523, 62.7258110046387",
	["Angel Pine : Vice City Maps"]     		    	= "0, -2177.60961914063, -2437.34497070313, 30.625",
	["Flint County : Nazca Symbol"]     		    	= "0, -687.696716308594, -1017.00006103516, 73.6602172851563",
	["Flint County : Suicidal Photographer"]    	    = "0, -61.9707489013672, -1622.80908203125, 3.51299381256104",
	["Red County : North Rock"]     		    		= "0, 2570.11279296875, -644.1416015625, 136.548034667969",
	["San Fierro : Actual Piece of Cable"]     	    	= "0, -2540.97973632813, 1228.51245117188, 37.421875",
	["San Fierro : Airport"]     			    		= "0, -1425.78784179688, -292.410827636719, 14.1484375",
	["San Fierro : Big Pointy Building"]     	    	= "0, -1754.5322265625, 885.199584960938, 295.875",
	["San Fierro : Bridge Facts"]     		    		= "0, -2556.43603515625, 1224.98254394531, 42.1484375",
	["San Fierro : Cargo Ship"]     		    		= "0, -2474.16943359375, 1547.40283203125, 33.2273330688477",
	["San Fierro : Gaydar Station Club"]     	    	= "0, -2557.06982421875, 192.114242553711, 6.1554012298584",
	["San Fierro : Highest building of San Fierro"]     = "0, -1791.02185058594, 567.774047851563, 332.801940917969",
	["San Fierro : Hippy Statue"]     		    		= "0, -2496.01245117188, -16.239330291748, 25.765625",
	["San Fierro : Hippy Shopper"]     		    		= "0, -2597.46752929688, 59.5114707946777, 4.3359375",
	["San Fierro : Michelle's Autorepair"]     	    	= "0, -1786.46594238281, 1197.98132324219, 25.1193962097168",
	["San Fierro : Misty's Club"]     		    		= "0, -2246.36303710938, -88.0900344848633, 35.3203125",
	["San Fierro : No Easter Eggs Up Here"]     	    = "0, -2677.59228515625, 1594.87243652344, 217.273895263672",
	["San Fierro : Otto's Autos"]     		    		= "0, -1639.01281738281, 1202.3388671875, 7.22374629974365",
	["San Fierro : USS Numnutz"]     		    		= "0, -1878.22204589844, 1455.84631347656, 8.36595344543457",
	["San Fierro : Wang Cars"]     		   	    		= "0, -1990.1318359375, 288.736297607422, 34.149341583252",
	["San Fierro : War Memorial"]     		    		= "0, -2591.06958007813, -15.1639175415039, 4.328125",
	["San Fierro : Zero's RC Shop"]     		    	= "0, -2245.77856445313, 128.705490112305, 35.3203125",
	["San Fierro : Zombotech Corporation"]     	    	= "0, -1921.11633300781, 721.838928222656, 45.4453125",
	["Chiliad Mountain : House on the slope"]     	    = "0, -2799.70458984375, -1528.5595703125, 139.274612426758",
	["Chiliad Mountain : Top of Chiliad Mountain"]      = "0, -2329.65673828125, -1624.35437011719, 483.707550048828",
	["Back o Beyond : Ghost Car"]    		    		= "0, -862.708190917969, -2280.87768554688, 26.2417907714844",
	["Las Venturas : Airport"]     			    		= "0, 1697.70825195313, 1436.81311035156, 10.7489976882935",
	["Las Venturas : Caligula's Palace Casino"]         = "0, 2179.212890625, 1676.203125, 11.046875",
	["Las Venturas : Caligula's Palace Casino basement"]= "0, 2170.4248046875, 1618.86999511719, 999.9765625",
	["Las Venturas : Casino Floor Casino"]     	    	= "0, 1658.6943359375, 2259.54443359375, 10.8203125",
	["Las Venturas : Club"]    			    			= "0, 2507.13940429688, 1247.13256835938, 10.8203125",
	["Las Venturas : Girls XXX Strip Club"]     	    = "0, 2506.6494140625, 2129.15478515625, 10.8203125",
	["Las Venturas : Lemmings Pedestrians"]     	    = "0, 2014.46105957031, 2356.97924804688, 10.8203125",
	["Las Venturas : Minigun - 01"]     		    	= "0, 2678.95947265625, 835.770690917969, 21.7657508850098",
	["Las Venturas : Minigun - 02"]     		    	= "0, 2495.65014648438, 2398.44189453125, 4.2109375",
	["Las Venturas : Minigun - 03"]     		    	= "0, 242.109191894531, 1859.30590820313, 14.0840120315552",
	["Las Venturas : Nude Strippers Daily Strip Club"]  = "0, 2543.13793945313, 1018.91442871094, 10.8125171661377",
	["Las Venturas : Police Departament"]    	    	= "0, 2286.89965820313, 2424.51220703125, 10.8203125",
	["Las Venturas : Sex shop"]     		    		= "0, 2096.162109375, 2077.07177734375, 10.8203125",
	["Las Venturas : The Craw Bar"]     		    	= "0, 2441.26611328125, 2059.05712890625, 10.8203125",
	["Las Venturas : The Four Dragons Casino"]     	    = "0, 2035.13293457031, 1007.79144287109, 10.8203125",
	["Las Venturas : VRock Hotel"]     		    		= "0, 2628.52221679688, 2321.35693359375, 10.671875",
	["Las Venturas : Wedding Chapel Open 24HR"]         = "0, 2434.20092773438, 1293.18017578125, 10.8203125",
	["Las Venturas : Wedding Chapel Open 7 Days"]       = "0, 2476.23413085938, 920.440734863281, 10.8203125",
	["Las Venturas : Welding & Weddings"]    	    	= "0, 2002.8212890625, 2288.66259765625, 10.8203125",
	["Bone County : Antena"]     			    		= "0, -225.078796386719, 1396.11413574219, 172.4140625",
	["Bone County : Lil' Probe Inn"]     		    	= "0, -84.36767578125, 1370.42724609375, 10.2734375",
	["Bone County : Area 69 (inside)"]     		    	= "0, 268.621276855469, 1887.26489257813, -29.53125",
	["Bone County : Astro Drive-In"]    		    	= "0, 148.567153930664, 1100.38610839844, 14.0299491882324",
	["Bone County : Cave"]     			    			= "0, -400.1201171875, 1233.3515625, 5.85399532318115",
	["Bone County : Los Brujas"]    		    		= "0, -373.940795898438, 2196.25903320313, 42.0821113586426",
	["Bone County : Regular Tom"]     		    		= "0, -318.049346923828, 1843.87377929688, 42.359375",
	["Bone County : The Big Ear"]     		    		= "0, -367.559143066406, 1581.53552246094, 76.1306381225586",
	["Bone County : The Big Ear (top)"]     	    	= "0, -346.331298828125, 1626.33447265625, 136.319305419922",
	["Bone County : The Big Spread Ranch Strip Club"]   = "0, 693.90673828125, 1940.20239257813, 5.5390625",
	["Verdant Meadows : Abandoned Airfield"]     	    = "0, 404.54638671875, 2488.81689453125, 16.484375",
	["El Castillo del Diablo : Snake Ferm"]     	    = "0, -36.4142532348633, 2336.65209960938, 24.1347389221191",
	["El Castillo del Diablo : Mass Grave"]     	    = "0, -132.202545166016, 2249.17138671875, 32.933177947998",
	["Fort Carson : Driveway Beagle"]     		    	= "0, -29.5530853271484, 1084.95336914063, 19.7455101013184",
	["The Sherman Dam : Generator Hall"]     	    	= "0, -593.485046386719, 2020.72277832031, 60.3828125",
	["The Sherman Dam : Sherman Dam Memorial"]          = "0, -889.46484375, 1972.90734863281, 60.3285140991211",
	["Las Payasadas : Largest Chicken"]     	    	= "0, -217.903747558594, 2662.41235351563, 62.6015625",
	["Las Barrancas : Largest Cow"]     		    	= "0, -857.239135742188, 1535.8515625, 28.1875",
	["Las Barrancas : Hopi Ruins"]     		    		= "0, -722.901062011719, 1522.61572265625, 39.5409965515137",
	["Tierra Robada : Aldea Malvada: the Lost Pueblo"]  = "0, -1298.81420898438, 2505.07299804688, 86.9625854492188",
	["Tierra Robada : Phalic Rock"]     		    	= "0, -547.985473632813, 2478.28100585938, 66.7793350219727",
	["Los Santos : Lemmings Pole"]     		    		= "0,  2392.33569335938, -1511.87316894531, 23.8348655700684",
	["Los Santos : Lowrider Competition"]     	    	= "0, 1816.08728027344, -1929.91040039063, 13.5433483123779",
	["Los Santos : Market Station"]     		    	= "0, 826.761840820313, -1358.68713378906, -0.5078125",
	["Los Santos : Observatory"]     		    		= "0, 1191.98645019531, -2036.88049316406, 69.0078125",
	["Los Santos : Ocean Docks"]     		    		= "0, 2184.40185546875, -2260.38232421875, 13.4100790023804",
	["Los Santos : Police brutality"]     		    	= "0, 1540.00646972656, -1627.77075195313, 13.3828125",
	["Los Santos : Police Departament"]     	    	= "0, 1540.84509277344, -1675.45324707031, 13.5513896942139",
	["Los Santos : Roboi's Food Mart"]     		    	= "0, 1350.62097167969, -1744.802734375, 13.3808469772339",
	["Los Santos : Stadium"]     		 	    		= "0, 2684.78198242188, -1689.56359863281, 9.43039894104004",
	["Los Santos : Ten Green Bottles"]     		    	= "0, 2304.39965820313, -1651.30908203125, 14.5202884674072",
	["Los Santos : The Pig Pen Strip Club"]     	    = "0, 2421.12280273438, -1228.55883789063, 24.8796482086182",
	["Los Santos : Tiki Theater"]     		    		= "0, 2348.78637695313, -1463.54272460938, 24",
	["Los Santos : Triathalon"]     		    		= "0, 181.06819152832, -1866.71862792969, 3.04375839233398",
	["Los Santos : Verona Beach Gym"]     		    	= "0, 666.700561523438, -1880.67272949219, 5.46000003814697",
	["Los Santos : Vinewood Sign"]     		    		= "0, 1386.48901367188, -822.793640136719, 73.6923141479492",
	["Los Santos : Vice City Hidden Package"]     	    = "0, 2354.03198242188, -1467.3642578125, 31.9559421539307",
	["Palomino Creek : Bank"]     			    		= "0, 2306.333984375, -15.3912973403931, 26.7495651245117",
	["Palomino Creek : Cementry"]     		    		= "0, 2240.06396484375, -61.6567649841309, 26.50661277771",
	["Palomino Creek : Empty Diner"]     		    	= "0, 2337.83618164063, 6.2173113822937, 26.484375",
	["Palomino Creek : Glowing headstones"]     	    = "0, 2240.09130859375, -70.5344467163086, 26.5068187713623",
	["Palomino Creek : Rusty Wheelchair"]     	    	= "0, 2123.24487304688, -90.2263793945313, 2.16398048400879",
	["Palomino Creek : Triathalon"]     		    	= "0, 2139.28759765625, -65.8627624511719, 2.96075963973999",
	["Montgomery : Biowaste Well"]     		    		= "0, 1277.96606445313, 292.559906005859, 19.5546875",
	["Montgomery : Inside Track Betting"]     	    	= "0, 1292.03247070313, 269.481689453125, 19.5546875",
	["Dillimore : Max Pane - Bulletproof Glass"]        = "0, 658.372924804688, -569.527587890625, 16.3359375",
	["Dillimore : Police Departament"]     		    	= "0, 635.456298828125, -571.612915039063, 16.3359375",
	["Blueberry : Liquor Store"]     		    		= "0, 254.497406005859, -66.0255889892578, 1.578125",
	["Blueberry : Locals Only Shop"]     		    	= "0, 276.548187255859, -157.536209106445, 1.5704517364502",
	["Fern Ridge : Catalina's Ex-boyfriends"]     	    = "0, 843.333190917969, -23.9582138061523, 62.7258110046387",
	["Angel Pine : Vice City Maps"]     		    	= "0, -2177.60961914063, -2437.34497070313, 30.625",
	["Flint County : Nazca Symbol"]     		    	= "0, -687.696716308594, -1017.00006103516, 73.6602172851563",
	["Flint County : Suicidal Photographer"]     	    = "0, -61.9707489013672, -1622.80908203125, 3.51299381256104",
	["Red County : North Rock"]     		    		= "0, 2570.11279296875, -644.1416015625, 136.548034667969",
	["San Fierro : Actual Piece of Cable"]     	    	= "0, -2540.97973632813, 1228.51245117188, 37.421875",
	["San Fierro : Airport"]     			    		= "0, -1425.78784179688, -292.410827636719, 14.1484375",
	["San Fierro : Big Pointy Building"]     	    	= "0, -1754.5322265625, 885.199584960938, 295.875",
	["San Fierro : Bridge Facts"]     		    		= "0, -2556.43603515625, 1224.98254394531, 42.1484375",
	["San Fierro : Cargo Ship"]     		    		= "0, -2474.16943359375, 1547.40283203125, 33.2273330688477",
	["San Fierro : Gaydar Station Club"]     	    	= "0, -2557.06982421875, 192.114242553711, 6.1554012298584",
	["San Fierro : Highest building of San Fierro"]     = "0, -1791.02185058594, 567.774047851563, 332.801940917969",
	["San Fierro : Hippy Statue"]     		    		= "0, -2496.01245117188, -16.239330291748, 25.765625",
	["San Fierro : Hippy Shopper"]     		    		= "0, -2597.46752929688, 59.5114707946777, 4.3359375",
	["San Fierro : Michelle's Autorepair"]     	    	= "0, -1786.46594238281, 1197.98132324219, 25.1193962097168",
	["San Fierro : Misty's Club"]     		    		= "0, -2246.36303710938, -88.0900344848633, 35.3203125",
	["San Fierro : No Easter Eggs Up Here"]     	    = "0, -2677.59228515625, 1594.87243652344, 217.273895263672",
	["San Fierro : Otto's Autos"]     		    		= "0, -1639.01281738281, 1202.3388671875, 7.22374629974365",
	["San Fierro : USS Numnutz"]     		    		= "0, -1878.22204589844, 1455.84631347656, 8.36595344543457",
	["San Fierro : Wang Cars"]     		  	    		= "0, -1990.1318359375, 288.736297607422, 34.149341583252",
	["San Fierro : War Memorial"]    		    		= "0, -2591.06958007813, -15.1639175415039, 4.328125",
	["San Fierro : Zero's RC Shop"]     		    	= "0, -2245.77856445313, 128.705490112305, 35.3203125",
	["San Fierro : Zombotech Corporation"]     	    	= "0, -1921.11633300781, 721.838928222656, 45.4453125",
	["Chiliad Mountain : House on the slope"]     	    = "0, -2799.70458984375, -1528.5595703125, 139.274612426758",
	["Chiliad Mountain : Top of Chiliad Mountain"]      = "0, -2329.65673828125, -1624.35437011719, 483.707550048828",
	["Back o Beyond : Ghost Car"]     		    		= "0, -862.708190917969, -2280.87768554688, 26.2417907714844",
	["Las Venturas : Airport"]     			    		= "0, 1697.70825195313, 1436.81311035156, 10.7489976882935",
	["Las Venturas : Caligula's Palace Casino"]         = "0, 2179.212890625, 1676.203125, 11.046875",
	["Las Venturas : Caligula's Palace Casino basement"]= "0, 2170.4248046875, 1618.86999511719, 999.9765625",
	["Las Venturas : Casino Floor Casino"]     	    	= "0, 1658.6943359375, 2259.54443359375, 10.8203125",
	["Las Venturas : Club"]     			    		= "0, 2507.13940429688, 1247.13256835938, 10.8203125",
	["Las Venturas : Girls XXX Strip Club"]     	    = "0, 2506.6494140625, 2129.15478515625, 10.8203125",
	["Las Venturas : Lemmings Pedestrians"]     	    = "0, 2014.46105957031, 2356.97924804688, 10.8203125",
	["Las Venturas : Minigun - 01"]     		    	= "0, 2678.95947265625, 835.770690917969, 21.7657508850098",
	["Las Venturas : Minigun - 02"]     		    	= "0, 2495.65014648438, 2398.44189453125, 4.2109375",
	["Las Venturas : Minigun - 03"]     		    	= "0, 242.109191894531, 1859.30590820313, 14.0840120315552",
	["Las Venturas : Nude Strippers Daily Strip Club"]  = "0, 2543.13793945313, 1018.91442871094, 10.8125171661377",
	["Las Venturas : Police Departament"]     	    	= "0, 2286.89965820313, 2424.51220703125, 10.8203125",
	["Las Venturas : Sex shop"]     		    		= "0, 2096.162109375, 2077.07177734375, 10.8203125",
	["Las Venturas : The Craw Bar"]     		    	= "0, 2441.26611328125, 2059.05712890625, 10.8203125",
	["Las Venturas : The Four Dragons Casino"]     	    = "0, 2035.13293457031, 1007.79144287109, 10.8203125",
	["Las Venturas : VRock Hotel"]     		    		= "0, 2628.52221679688, 2321.35693359375, 10.671875",
	["Las Venturas : Wedding Chapel Open 24HR"]         = "0, 2434.20092773438, 1293.18017578125, 10.8203125",
	["Las Venturas : Wedding Chapel Open 7 Days"]       = "0, 2476.23413085938, 920.440734863281, 10.8203125",
	["Las Venturas : Welding & Weddings"]     	    	= "0, 2002.8212890625, 2288.66259765625, 10.8203125",
	["Bone County : Antena"]     			    		= "0, -225.078796386719, 1396.11413574219, 172.4140625",
	["Bone County : Lil' Probe Inn"]     		    	= "0, -84.36767578125, 1370.42724609375, 10.2734375",
	["Bone County : Area 69 (inside)"]     		    	= "0, 268.621276855469, 1887.26489257813, -29.53125",
	["Bone County : Astro Drive-In"]     		    	= "0, 148.567153930664, 1100.38610839844, 14.0299491882324",
	["Bone County : Cave"]     			    			= "0, -400.1201171875, 1233.3515625, 5.85399532318115",
	["Bone County : Los Brujas"]     		    		= "0, -373.940795898438, 2196.25903320313, 42.0821113586426",
	["Bone County : Regular Tom"]     		    		= "0, -318.049346923828, 1843.87377929688, 42.359375",
	["Bone County : The Big Ear"]     		    		= "0, -367.559143066406, 1581.53552246094, 76.1306381225586",
	["Bone County : The Big Ear (top)"]     	    	= "0, -346.331298828125, 1626.33447265625, 136.319305419922",
	["Bone County : The Big Spread Ranch Strip Club"]   = "0, 693.90673828125, 1940.20239257813, 5.5390625",
	["Verdant Meadows : Abandoned Airfield"]     	    = "0, 404.54638671875, 2488.81689453125, 16.484375",
	["El Castillo del Diablo : Snake Ferm"]     	    = "0, -36.4142532348633, 2336.65209960938, 24.1347389221191",
	["El Castillo del Diablo : Mass Grave"]     	    = "0, -132.202545166016, 2249.17138671875, 32.933177947998",
	["Fort Carson : Driveway Beagle"]     		    	= "0, -29.5530853271484, 1084.95336914063, 19.7455101013184",
	["The Sherman Dam : Generator Hall"]     	    	= "0, -593.485046386719, 2020.72277832031, 60.3828125",
	["The Sherman Dam : Sherman Dam Memorial"]     	    = "0, -889.46484375, 1972.90734863281, 60.3285140991211",
	["Las Payasadas : Largest Chicken"]     	    	= "0, -217.903747558594, 2662.41235351563, 62.6015625",
	["Las Barrancas : Largest Cow"]     		    	= "0, -857.239135742188, 1535.8515625, 28.1875",
	["Las Barrancas : Hopi Ruins"]     		    		= "0, -722.901062011719, 1522.61572265625, 39.5409965515137",
	["Tierra Robada : Aldea Malvada: the Lost Pueblo"]  = "0, -1298.81420898438, 2505.07299804688, 86.9625854492188",
	["Tierra Robada : Phalic Rock"]     		    	= "0, -547.985473632813, 2478.28100585938, 66.7793350219727",
	["Liberty City : Marco's Bistro"]     		    	= "1, -748.254211425781, 488.972473144531, 1371.58898925781",
	["Liberty City : Inside Marco's Bistro"]     	    = "1, -781.811340332031, 489.323181152344, 1376.1953125",
	["Interior : Planning Department"]     	    	    = "3, 388.872009277344, 173.811111450195, 1008.3828125",
	["Interior : Barn"]     			    			= "3, 291.141082763672, 310.135040283203, 999.1484375",
	["Interior : Millie's House"]     		    		= "6, 345.892669677734, 306.162902832031, 999.1484375",
	["Interior : Cafe"]     			    			= "5, 450.560333251953, -107.105247497559, 1000.88635253906",
	["Interior : Place A"]     			    			= "3, 1041.75390625, 10.1560220718384, 1001.28448486328",
	["Interior : Place B"]     			    			= "3, 959.504577636719, -7.69411325454712, 1001.1484375",
	["Interior : Place C"]     			    			= "3, 962.10498046875, -50.9754791259766, 1001.1171875",
}

function module.Teleport(x, y, z,interior_id)
	if x == nil then
		_, x,y,z = getTargetBlipCoordinates()
		interior_id = 0
	end
	if tteleport.auto_z[0] then
		z = getGroundZFor3dCoord(x, y, z)
	end

	if x ~= 0 and y ~= 0 then
		setCharInterior(PLAYER_PED,interior_id)
		setInteriorVisible(interior_id)
		clearExtraColours(true)
		requestCollision(x,y)
		loadScene(x,y,z)
		activateInteriorPeds(true)
        setCharCoordinates(PLAYER_PED, x, y, z)
    end
end


function ShowTeleportEntry(label, x, y, z,interior_id)
	if imgui.MenuItemBool(label) then
		module.Teleport(x, y, z,interior_id)
	end
end

function module.TeleportMain()

    if imgui.BeginTabBar("Teleport") then
        if imgui.BeginTabItem(flanguage.GetText("teleport.Teleport")) then
            imgui.Spacing()
            imgui.Columns(2,nil,false)
            fcommon.CheckBox({name = flanguage.GetText("teleport.QuickTeleport"),var = fteleport.tteleport.shortcut,help_text =flanguage.GetText("teleport.QuickTeleportToolTip")})
            fcommon.CheckBox({name = flanguage.GetText("teleport.AutoZCoordinates"),var = fteleport.tteleport.auto_z,help_text =flanguage.GetText("teleport.AutoZCoordinatesToolTip")})
            imgui.NextColumn()
            fcommon.CheckBox({name = flanguage.GetText("teleport.InsertCoordinates"),var = fteleport.tteleport.insert_coords,help_text =flanguage.GetText("teleport.InsertCoordinatesToolTip")})
            imgui.Columns(1)

            if imgui.InputText(flanguage.GetText("teleport.Coordinates"),tteleport.coords,ffi.sizeof(tteleport.coords)) then end

            if tteleport.insert_coords[0] then
                local x,y,z = getCharCoordinates(PLAYER_PED)
                imgui.StrCopy(tteleport.coords,string.format("%d, %d, %d", math.floor(x) , math.floor(y) , math.floor(z)))
            end

            fcommon.InformationTooltip(flanguage.GetText("teleport.InputToolTip"))
            imgui.Dummy(imgui.ImVec2(0,10))

            if imgui.Button(flanguage.GetText("teleport.TeleportToCoord"),imgui.ImVec2(fcommon.GetSize(2))) then
                local x,y,z = (ffi.string(tteleport.coords)):match("([^,]+),([^,]+),([^,]+)")
                module.Teleport(x, y, z,0)
            end
            imgui.SameLine()
            if imgui.Button(flanguage.GetText("teleport.TeleportToMarker"),imgui.ImVec2(fcommon.GetSize(2))) then
                module.Teleport()
            end
            imgui.EndTabItem()
        end
        if imgui.BeginTabItem('Search') then
            imgui.Spacing()
            imgui.Columns(1)
            if imgui.InputText(flanguage.GetText("common.Search"),tteleport.search_text,ffi.sizeof(tteleport.search_text)) then end
			imgui.SameLine()

			imgui.Spacing()
			imgui.Text(flanguage.GetText("common.FoundEntries") .. ":(" .. ffi.string(tteleport.search_text) .. ")")
			imgui.Separator()
			imgui.Spacing()
			if imgui.BeginChild("Teleport entries") then
				for name, coord in pairs(coordinates) do
					local interior_id, x, y, z = coord:match("([^, ]+), ([^, ]+), ([^, ]+), ([^, ]+)")
					if ffi.string(tteleport.search_text) == "" then
						ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
					else
						if string.upper(name):find(string.upper(ffi.string(tteleport.search_text))) ~= nil  then
							ShowTeleportEntry(name, tonumber(x), tonumber(y), tonumber(z),interior_id)
						end
					end
				end
				imgui.EndChild()
			end
            imgui.EndTabItem()
        end
        imgui.EndTabBar()
    end
end

return module
