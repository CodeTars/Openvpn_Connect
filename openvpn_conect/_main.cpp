#include "_common.h"

static time_t tic, toc, tt;
static char loc_time[20];
static string cmd, line, name, op;
static int ip_nums, fcnt, SECS = 15, WAIT = 6;
static bool isPick = false, isUpdate = false;

static fstream settings;
static ifstream tmp, list;
static ofstream fail, info;
static struct IP{
	string name;
	bool useful;
	int ms = 1000000;
}ip[50];

static bool cmp(IP a, IP b) {
	return a.ms < b.ms;
}

int main(int argc, char **argv) {
	/* 处理参数 */
    {
		-- argc;
		if(argc == 0) {
			connect(0);
			return 0;
		}
		if(argc && argv[1][0] != '-') {
			system("openvpn-gui --command disconnect_all");
			char cmd[] = "start openvpn-gui --connect ";
			system(strcat(cmd, argv[1]));
			return 0;
		}
	    for(int i = 1; i <= argc; i++) {
	    	switch (argv[i][1]) {
	    		/* -a -b -? */
			    case 'a':
			        connect(argv[i]);
			        return 0;
			    case 'b':
			        connect(argv[i]);
			        return 0;
			    default:
			        return 0;
	            case 'o': 
			        system("start openvpn-gui");
			        return 0;
	    		case 'd':
				    system("start openvpn-gui --command disconnect_all");
				    return 0;
				case 'e':
				    system("start openvpn-gui --command disconnect_all");
				    system("openvpn-gui --command exit");
				    return 0;
			    /* ---------------------- */
	            case 's':
	                break;
	    		case 't':
	    		    ++ i;
	    		    SECS = atoi(argv[i]);
	    		    if(SECS == 0) {
	    		    	SECS = 15;
	    		    }
	    		    break;
			    case 'p':
			        isPick = true;
			        break;
			    case 'u':
			        isUpdate = true;
			        break;
			    case 'w':
			    	++ i;
			    	WAIT = atoi(argv[i]);
			    	if(WAIT == 0) {
			    		WAIT = 6;
			    	}
			    	break;
	    	}
	    }
    }

    tic = time(0);
    /* 生成清单 */
	{
		if(isUpdate) {
			string num, ms;
			list.open("_节点.txt");
			getline(list, line);
		    while(list >> num >> ip[ip_nums].name >> ms) ++ ip_nums;
			list.close();
		} else {
	        system("DIR *.ovpn/B > 节点清单.txt");
	        if(isPick) {
	    	    system("节点清单.txt");
		    }
			list.open("节点清单.txt");
			while (getline(list, name) && name != "") {
				ip[ip_nums++].name = name.substr(0, name.length() - 5);
			}
			list.close();
			system("del 节点清单.txt");
		}  
	}   

	/* 连接并测速 */
	for(int i = 0; i < ip_nums; i++) {
		system("openvpn-gui --command disconnect_all");
		name = ip[i].name;
		cout << name << "(" << i+1 << "/" << i-fcnt << "/" << ip_nums << ")" << endl;
		/* password */
		{
			settings.open(name);
			settings.open(name + ".ovpn");
			while(getline(settings, line)) {
				if(line == "auth-user-pass") {
					settings.seekp(-1, ios::cur);
					settings << " _paswd.txt";
					break;
				}
			}
			settings.close();
			cout << "    Set password sucessfully!" << endl;
		}
		/* connect */
		{
			ip[i].useful = false;
			cmd = "start openvpn-gui --connect " + name;
			system(cmd.c_str());
			cout << "    connecting";
			for(int t = 0; t < SECS; t++) {
				Sleep(1000);
				cout << ".";
				system("netsh interface show interface name=\"OpenVPN TAP-Windows6\" > tmp");
				tmp.open("tmp");
			    for(int l = 0; l < 5; l++) { getline(tmp, line); }
			    tmp.close();
			    if(line.find("已连接") != string::npos) {
			    	cout << "\n    " << t + 1 << "s, Sucecced";
			    	ip[i].useful = true;
			    	break;
			    }
			}
		}
		/* 测速 */
		{
			if(ip[i].useful) {
				for(int t = 0; t < WAIT; t++) {
					Sleep(1000);
					cout << ".";
				}
				cout << endl << "    Speed test..." << endl;
			    system("ping www.youtube.com > tmp");
			    tmp.open("tmp");
			    for(int l = 0; getline(tmp, line); l++) {
			    	if(l == 8 || l == 10) {
			    		cout << line << endl;
		    		}
		    		if(l == 10) {
		    			ip[i].ms = stoi(line.substr(line.length() - 6, 4));
		    		}
			    	
			    }
			    tmp.close();
			} else {
				++ fcnt;
				cout << "\n    Failed! The node is listed as invalid." << endl;
			}
		}
	}
	system("del tmp");


    /* 总结 */
	{
		toc = time(0);
		cout << "-----------------------------------------------------" << endl
		     << "历时" << setw(2) << (toc - tic) / 60 << "分" << setw(2) << (toc - tic) % 60 << "秒, " 
		     << "共对" << setw(2) << ip_nums << "个节点进行了" << "测速, " 
		     << "有" << setw(2) << fcnt << "个节点无效。" << endl;
        if(fcnt == ip_nums) {
        	cout << endl;
        	system("pause");
        	return 0;
        }
		sort(ip, ip + ip_nums, cmp);
		cout << "下面是速度最快的几个节点: " << endl;
		for(int i = 0; i < 5 && ip[i].useful; i++) {
			cout << left << setw(17) <<ip[i].name 
			     << "   " << right << setw(4) << ip[i].ms << "ms" << endl;
		}
		cout << "-----------------------------------------------------" << endl
		     << "连入" << ip[0].name << "." << endl; 
		cmd = "start openvpn-gui --connect " + ip[0].name;
		system("openvpn-gui --command disconnect_all");
		system(cmd.c_str());
	}

	/* 节点信息录入 */
	{
		cout << "是否写入新的节点排序？";
		getline(cin, op);
		if(op.length()) {
			return 0;
		}
		if(access("_节点.txt", 0) == 0) {
			cout << endl;
			system("type _节点.txt");
		}
		info.open("_节点.txt");
		tt = time(0);
		tm *now = localtime(&tt);
		sprintf(loc_time, "%d/%02d/%02d %02d:%02d", 1900 + now->tm_year, 1 + now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min);
		info << "-------" << loc_time << "-------" << endl;
		for(int i = 0; i < ip_nums - fcnt; i++) {
			info << right << setw(2) << i + 1 << "  " << left << setw(17) << ip[i].name 
			     << "   " << right << setw(4) << ip[i].ms << "ms" << endl;
		}
		info << "------------------------------" << endl;
		info.close();
		cout << endl; 
		system("type _节点.txt");
		cout << endl;
	}

	system("pause");
	return 0;
}
