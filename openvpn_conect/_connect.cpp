#include "_common.h"

static int cnt;
static ifstream info;
static string line[100];

static int get_info(string s = "_节点.txt", int cnt = 0) {
    info.open(s);
    while(getline(info, line[cnt]) 
        && line[cnt] != "" && line[cnt] != string(30, '-')) ++cnt;
    info.close();
    return cnt - 1;
}

void connect(char* op) {
	bool all = false;
	string cmd;
	if(op) {
		switch (op[1]) {
			case 'a':
			    system("DIR *.ovpn/B > tmp");
			    cnt = get_info("tmp", 1);
			    system("del tmp");
			    all = true;
			    break;	
			case 'b':
			    get_info();
			    system("openvpn-gui --command disconnect_all");
			    cmd = "start openvpn-gui --connect " + line[1].substr(4, 17);
			    system(cmd.c_str());
			    return;
		}
	}
	if(!all) {
		cnt = get_info();
		for(int i = 0; i < cnt + 2; i++) {
	        cout << line[i] << endl;
		}
	} else {
		for(int i = 1; i <= cnt; i++) {
            cout << setw(2) << i << "  " 
                 << line[i].substr(0, line[i].length() - 5) << endl;
		}
		cout << string(21, '-') << endl;
	}
    
	int i;
    cout << "输入编号连接节点 ";
    getline(cin, cmd);
    try {
        i = stoi(cmd);
        if(i < 1 || i > cnt) {
	       return;
        }
    } catch (...) {
        return;
    }
    string ip = all ? line[i].substr(0, line[i].length() - 5) : 
               line[i].substr(4,17);
	cout << "正在连入" << ip << endl;
	system("openvpn-gui --command disconnect_all");
	cmd = "start openvpn-gui --connect " + ip;
	system(cmd.c_str());
	Sleep(300);
	return;
}
