#include "volume.h"
#include "utils.h"
#include "RSA.h"
#include "Entry.h"

unsigned int fileEnd = UINT32_MAX;

void Volume::init(int size, string password) {
	//Tạo MyFS.dat kích thước size
	data.open("MyFS.dat", ios::out | ios::binary);
	data.seekp(size);
	char temp = 0;
	data.write(&temp, 1);

	//Tính số Sector
	int sectors = size / 512;

	//Xác định các thông số
	signature[0]='H';
	signature[1] = 'T';
	signature[2] = 'T';
	bytePerSector = 512;
	numberOfFat = 2;
	fatStartSector = 1;
	sectorPerCluster = 4;
	rdetCluster = 3;

	//Tính kích thước bảng FAT và số cluster
	numberOfCluters = (sectors - fatStartSector - numberOfFat) / (4 * numberOfFat / 512 + sectorPerCluster);//4 là số byte quản lý 1 cluster
	sectorPerFat = (4 * numberOfCluters) / 512 + 1;

	publicKey = 17;
	string pwd_mahoa = RSA_pwd::encryptPassword(password);
	passWordLenght = pwd_mahoa.length();
	passWord = pwd_mahoa;

	//Ghi các thông tin vào offset tương ứng
	char* buffer = new char[512];
	for (int i = 0; i < 512; i++) {
		buffer[i] = 0;
	}
	writeOffset(buffer, 0, signature, 4);
	writeOffset(buffer, 4, (char*)&bytePerSector, sizeof(bytePerSector));
	writeOffset(buffer, 6, (char*)&numberOfFat, sizeof(numberOfFat));
	writeOffset(buffer, 7, (char*)&fatStartSector, sizeof(fatStartSector));
	writeOffset(buffer, 8, (char*)&sectorPerFat, sizeof(sectorPerFat));
	writeOffset(buffer, 10, (char*)&sectorPerCluster, sizeof(sectorPerCluster));
	writeOffset(buffer, 11, (char*)&rdetCluster, sizeof(rdetCluster));
	writeOffset(buffer, 15, (char*)&numberOfCluters, sizeof(numberOfCluters));
	writeOffset(buffer, 19, (char*)&publicKey, sizeof(publicKey));
	writeOffset(buffer, 20, (char*)&passWordLenght, sizeof(passWordLenght));
	writeOffset(buffer, 21, (char*)(passWord.c_str()), passWordLenght);

	writeBlock(0, buffer); // ghi sector 0

	for (int i = 0; i < 512; i++) {
		buffer[i] = 0;
	}
	
	//Ghi File RDET vào bảng FAT
	writeOffset(buffer, 4 * rdetCluster, (char*)&fileEnd, 4);
	writeBlock(1, buffer);

	for (int i = 0; i < 512; i++) {
		buffer[i] = 0;
	}

	//Ghi các sector còn lại 
	for (int i = 2; i < sectors; i++) {
		writeBlock(i, buffer);
	}

	//Ghi đè FAt 2
	writeBlock(1 + sectorPerFat, buffer);
	data.close();
}

bool Volume::open(string passwordInput) {
	data.open("MyFS.dat", ios::binary | ios::out | ios::in);
	if (!data.is_open()) {
		cout << "Volume chua khoi tao" << endl;
		return false;
	}

	//Đọc sector 0 lấy thông tin bootsector
	char* buffer = readBlock(0);
	//Lấy các thông số từ sector 0
	bytePerSector = *(uint16_t*)&buffer[4];
	numberOfFat = *(uint8_t*)&buffer[6];
	fatStartSector = *(uint8_t*)&buffer[7];
	sectorPerFat = *(uint16_t*) &buffer[8];
	sectorPerCluster = *(uint8_t*)&buffer[10];
	rdetCluster = *(unsigned int*)&buffer[11];
	numberOfCluters = *(unsigned int*)&buffer[15];
	publicKey = buffer[19];
	passWordLenght = buffer[20];
	for (int i = 0; i < passWordLenght; i++) {
		passWord += buffer[21 + i];
	}
	string pwd_giaima = RSA_pwd::decryptPassword(passWord);
	if (pwd_giaima != passwordInput) {
		cout << "Mat khau khong dung" << endl;
		return false;
	}
	delete buffer;
	return true;
};

char* Volume::readBlock(int num) {
	char* result = new char[512];

	this->data.seekg(num* 512);
	this->data.read(result, 512);


	return result;
};

void Volume::writeBlock(int num, char* data) {
	this->data.seekp(num * 512);
	this->data.write(data, 512);
};

unsigned int* Volume::readFat() {
	unsigned int* result = new unsigned int [numberOfCluters + 3];
	int index = 0;

	for (int n = 0; n < sectorPerFat; n++) {
		char*buffer = readBlock(fatStartSector + n);
		for (int i = 0; i < 512; i+=4) {
			result[index] = *(unsigned int*)( &buffer[i]);
			index++;
			if (index == numberOfCluters + 3 ) {
				break;
			}
		}
		delete[] buffer;
	}
	
	return result;
};

void Volume:: writeFat(unsigned int* a) {
	//ghi bảng FAT 1
	for (int i = 0; i < sectorPerFat; i++) {
		char* temp = (char*)&a[i*(512/4)];
		writeBlock(fatStartSector + i, temp);
	}
	//ghi bảng FAT2
	for (int i = 0; i < sectorPerFat; i++) {
		char* temp = (char*)&a[i * (512 / 4)];
		writeBlock(fatStartSector+sectorPerFat + i, temp);
	}
}

char* Volume::readCluster(int num) {
	char* result = new char[512 * sectorPerCluster];

	this->data.seekg(512*(fatStartSector + sectorPerFat * numberOfFat + (num - 3) * sectorPerCluster));
	data.read(result, 512 * sectorPerCluster);

	return result;
};

void Volume::writeCluster(int num, char* data) {
	this->data.seekp(512*(fatStartSector + sectorPerFat * numberOfFat + (num - 3) * sectorPerCluster));
	this->data.write(data, 512 * sectorPerCluster);
};

bool Volume::resetPassWord(string newPW) {
	char* buffer = readBlock(0);
	//Xoa mat khau cu tren bootsector
	for (int i = 0; i < passWordLenght; i++) {
		buffer[21 + i] = 0;
	}

	//Luu mat khau moi vao bootsector
	string pwd_mahoa = RSA_pwd::encryptPassword(newPW);
	passWordLenght = pwd_mahoa.length();
	passWord = pwd_mahoa;
	writeOffset(buffer, 20, (char*)&passWordLenght, sizeof(passWordLenght));
	writeOffset(buffer, 21, (char*)(passWord.c_str()), passWordLenght);


	writeBlock(0, buffer);
	delete[] buffer;

	return true;
}

void Volume::updatePassword(string filename) {
	string oldPassword, newPassword;
	bool isExist = false;
	vector<Entry> listEntry = this->readRDET();
	Entry EntryOfFilename;

	// check file name tồn tại hay không nếu tồn tại thì lấy entry đó ra
	for (int i = 0; i < listEntry.size(); i++) {
		if (filename == listEntry[i].getFileName()) {
			isExist = true;
			EntryOfFilename = listEntry[i];
			break;
		}
	}

	if (!isExist) {
		cout << filename << " khong ton tai!" << endl;
		return;
	}

	unsigned int* FAT_table = this->readFat();

	vector<int> RDET_cluster;
	RDET_cluster.push_back(rdetCluster);

	while (1) {
		if (FAT_table[RDET_cluster[RDET_cluster.size() - 1]] == fileEnd) {
			break;
		}

		RDET_cluster.push_back(FAT_table[RDET_cluster[RDET_cluster.size() - 1]]);
	}

	// TH khong co password
	if (EntryOfFilename.getPassword() == "") {
		// nhap mat khau
		cin.ignore();
		cout << "Vui long nhap mat khau de thiet lap: ";
		getline(cin, newPassword);

		// tính lại kích thước Entry do kích thước password đã thay đổi
		uint8_t newSizeOfEntry = EntryOfFilename.getSize() - EntryOfFilename.getPassSize() + newPassword.length();

		//// tạo Entry mới
		Entry newEntry;
		newEntry.createEntry(filename, newPassword, EntryOfFilename.getDataSize(), EntryOfFilename.getType(), EntryOfFilename.getStartCluster());

		int countMoreEmpty = 0;
		int isEnough = false;
		int byteStartWrite;
		for (int i = 0; i < RDET_cluster.size(); i++) {
			char* clusterData = readCluster(RDET_cluster[i]);
			int j = 0;
			while (j < 512 * sectorPerCluster) {
				char val = *(char*)(&clusterData[j]);
				if (val == 0) {
					j++;
				}
				else {
					Entry entry;
					entry.readEntry(&clusterData[j]);
					if (entry.getFileName() == filename && entry.getSize() == EntryOfFilename.getSize()) {
						// kiểm tra thử phía sau có còn đủ không gian để nối vào lưu tiếp hay không
						countMoreEmpty = val;
						byteStartWrite = j;
						while (1) {
							char elm = *(char*)(&clusterData[j]);
							if (elm == 0) {
								countMoreEmpty++;
								if (countMoreEmpty == newSizeOfEntry) {
									isEnough = true;
									break;
								}
							}
							j++;

						}

						if (isEnough) {
							char* cluster = readCluster(RDET_cluster[i]);
							writeOffset(cluster, byteStartWrite, newEntry.toBytes(), newSizeOfEntry);
							writeCluster(RDET_cluster[i], cluster);
						}
						break;
					}
					j += val;
				}
			}
			delete[] clusterData;
		}

		if (!isEnough) {
			// Tìm vị trí lưu Entry mới 
			int clusterEntry; // cluster của Entry
			int offsetEntry; // offset của Entry trên cluster đó

			//// truy xuất RDET

			char* temp = NULL;
			int countByteEmpty = 0;
			bool flag = false;

			int i = 0, j = 0;
			while (i < RDET_cluster.size()) {
				temp = readCluster(RDET_cluster[i]);
				while (j < sectorPerCluster * 512) {
					char val = *(char*)(&temp[j]);
					if (val == 0) {
						countByteEmpty++;
						if (countByteEmpty == newSizeOfEntry) {
							flag = true;
							break;
						}
						j++;
					}
					else {
						j = j + val;
					}
				}
				delete[] temp;

				if (!flag) {
					i++;
				}
				else {
					break;
				}
			}


			// không đủ N byte liên tiếp
			if (!flag) {
				//Tìm cluster trống
				int indexClusterEmpty = -1;
				for (int i = 3; i < numberOfCluters + 3; i++) {
					if (FAT_table[i] == 0) {
						indexClusterEmpty = i;
						break;
					}
				}

				// mở rộng bảng RDET
				if (indexClusterEmpty == -1) {
					cout << "Khong du kich thuoc de luu file" << endl;
					return;
				}
				else {
					FAT_table[RDET_cluster[RDET_cluster.size() - 1]] = indexClusterEmpty;
					FAT_table[indexClusterEmpty] = fileEnd;

					clusterEntry = indexClusterEmpty;
					offsetEntry = 0;
				}
			}
			else {
				clusterEntry = RDET_cluster[i];
				offsetEntry = j - newSizeOfEntry + 1;
			}

			// ghi giá 0 lên các byte của Entry cũ
			for (int i = 0; i < RDET_cluster.size(); i++) {
				char* clusterData = readCluster(RDET_cluster[i]);
				int j = 0;
				while (j < 512 * sectorPerCluster) {
					char val = *(char*)(&clusterData[j]);
					if (val == 0) {
						j++;
					}
					else {
						Entry entry;
						entry.readEntry(&clusterData[j]);
						if (entry.getFileName() == filename && entry.getSize() == EntryOfFilename.getSize()) {
							char* emptyEntry = entry.toBytes();
							for (int a = 0; a < entry.getSize(); a++) {
								emptyEntry[a] = 0;
							}

							// ghi toàn bộ giá trị 0 cho các byte của Entry cũ
							char* cluster = readCluster(RDET_cluster[i]);
							writeOffset(cluster, j, emptyEntry, entry.getSize());
							writeCluster(RDET_cluster[i], cluster);

							delete[] cluster, clusterData, emptyEntry;
							break;
						}
						j += val;
					}
				}
				delete[] clusterData;
			}

			// cập nhật Entry vào RDET
			char* cluster = readCluster(clusterEntry);
			writeOffset(cluster, offsetEntry, newEntry.toBytes(), newSizeOfEntry);
			writeCluster(clusterEntry, cluster);
			delete[] cluster;

			// cập nhật bảng FAT
			writeFat(FAT_table);
		}
	}
	else { // TH co mat khau
		cin.ignore();
		cout << "Vui long nhap mat khau cu: ";
		getline(cin, oldPassword);

		// TH mat khau khong khop
		if (!EntryOfFilename.checkPassword(oldPassword)) {
			cout << "Mat khau khong khop!" << endl;
			return;
		}

		cout << "Mat khau khop ..." << endl;
		cout << "Vui long nhap mat khau moi: ";
		getline(cin, newPassword);

		// Tao Entry với những thông tin mới
		Entry newEntry;
		newEntry.createEntry(filename, newPassword, EntryOfFilename.getDataSize(), EntryOfFilename.getType(), EntryOfFilename.getStartCluster());

		//TH password mới có cùng kích thước
		if (newPassword.length() == EntryOfFilename.getPassword().length()) {
			// cập nhật Entry vào RDET
			for (int i = 0; i < RDET_cluster.size(); i++) {
				char* clusterData = readCluster(RDET_cluster[i]);
				int j = 0;
				while (j < 512 * sectorPerCluster) {
					char val = *(char*)(&clusterData[j]);
					if (val == 0) {
						j++;
					}
					else {
						Entry entry;
						entry.readEntry(&clusterData[j]);
						if (entry.getFileName() == filename && entry.getSize() == EntryOfFilename.getSize()) {
							char* cluster = readCluster(RDET_cluster[i]);
							writeOffset(cluster, j, newEntry.toBytes(), entry.getSize());
							writeCluster(RDET_cluster[i], cluster);

							delete[] cluster, clusterData;
							break;
						}
						j += val;
					}
				}
				delete[] clusterData;
			}
		}
		else { // TH mật khẩu mới khác kích thước
			// tính lại kích thước Entry do kích thước password đã thay đổi
			uint8_t newSizeOfEntry = EntryOfFilename.getSize() - EntryOfFilename.getPassSize() + newPassword.length();

			// cap nhat RDET
			int countMoreEmpty = 0;
			int isEnough = false;
			int byteStartWrite;
			for (int i = 0; i < RDET_cluster.size(); i++) {
				char* clusterData = readCluster(RDET_cluster[i]);
				int j = 0;
				while (j < 512 * sectorPerCluster) {
					char val = *(char*)(&clusterData[j]);
					if (val == 0) {
						j++;
					}
					else {
						Entry entry;
						entry.readEntry(&clusterData[j]);
						if (entry.getFileName() == filename && entry.getSize() == EntryOfFilename.getSize()) {
							// kiểm tra thử phía sau có còn đủ không gian để nối vào lưu tiếp hay không
							if (entry.getSize() < newSizeOfEntry) {
								countMoreEmpty = val;
								byteStartWrite = j;
								while (1) {
									char elm = *(char*)(&clusterData[j]);
									if (elm == 0) {
										countMoreEmpty++;
										if (countMoreEmpty == newSizeOfEntry) {
											isEnough = true;
											break;
										}
									}
									j++;
								}

								if (isEnough) {
									char* cluster = readCluster(RDET_cluster[i]);
									writeOffset(cluster, byteStartWrite, newEntry.toBytes(), newSizeOfEntry);
									writeCluster(RDET_cluster[i], cluster);

									delete[] cluster, clusterData;
								}
							}
							else {
								isEnough = true;
								char* cluster = readCluster(RDET_cluster[i]);
								writeOffset(cluster, j, newEntry.toBytes(), newSizeOfEntry);
								writeCluster(RDET_cluster[i], cluster);

								delete[] cluster, clusterData;
							}
							break;
						}
						j += val;
					}
				}
				delete[] clusterData;
			}

			if (!isEnough) {
				// Tìm vị trí lưu Entry mới 
				int clusterEntry; // cluster của Entry
				int offsetEntry; // offset của Entry trên cluster đó

				//// truy xuất RDET

				char* temp = NULL;
				int countByteEmpty = 0;
				bool flag = false;

				int i = 0, j = 0;
				while (i < RDET_cluster.size()) {
					temp = readCluster(RDET_cluster[i]);
					while (j < sectorPerCluster * 512) {
						char val = *(char*)(&temp[j]);
						if (val == 0) {
							countByteEmpty++;
							if (countByteEmpty == newSizeOfEntry) {
								flag = true;
								break;
							}
							j++;
						}
						else {
							j = j + val;
						}
					}
					delete[] temp;

					if (!flag) {
						i++;
					}
					else {
						break;
					}
				}

				// không đủ N byte liên tiếp
				if (!flag) {
					//Tìm cluster trống
					int indexClusterEmpty = -1;
					for (int i = 3; i < numberOfCluters + 3; i++) {
						if (FAT_table[i] == 0) {
							indexClusterEmpty = i;
							break;
						}
					}

					// mở rộng bảng RDET
					if (indexClusterEmpty == -1) {
						cout << "Khong du kich thuoc de luu file" << endl;
						return;
					}
					else {
						FAT_table[RDET_cluster[RDET_cluster.size() - 1]] = indexClusterEmpty;
						FAT_table[indexClusterEmpty] = fileEnd;

						clusterEntry = indexClusterEmpty;
						offsetEntry = 0;
					}
				}
				else {
					clusterEntry = RDET_cluster[i];
					offsetEntry = j - newSizeOfEntry + 1;
				}

				// ghi giá 0 lên các byte của Entry cũ
				for (int i = 0; i < RDET_cluster.size(); i++) {
					char* clusterData = readCluster(RDET_cluster[i]);
					int j = 0;
					while (j < 512 * sectorPerCluster) {
						char val = *(char*)(&clusterData[j]);
						if (val == 0) {
							j++;
						}
						else {
							Entry entry;
							entry.readEntry(&clusterData[j]);
							if (entry.getFileName() == filename && entry.getSize() == EntryOfFilename.getSize()) {
								char* emptyEntry = entry.toBytes();
								for (int a = 0; a < entry.getSize(); a++) {
									emptyEntry[a] = 0;
								}

								// ghi toàn bộ giá trị 0 cho các byte của Entry cũ
								char* cluster = readCluster(RDET_cluster[i]);
								writeOffset(cluster, j, emptyEntry, entry.getSize());
								writeCluster(RDET_cluster[i], cluster);

								delete[] cluster, clusterData, emptyEntry;
								break;
							}
							j += val;
						}
					}
					delete[] clusterData;
				}

				// cập nhật Entry vào RDET
				char* cluster = readCluster(clusterEntry);
				writeOffset(cluster, offsetEntry, newEntry.toBytes(), newSizeOfEntry);
				writeCluster(clusterEntry, cluster);
				delete[] cluster;

				// cập nhật bảng FAT
				writeFat(FAT_table);
			}
		}
	}

	delete[] FAT_table;
}

void Volume::import(string path) {

	// Bước 2
	fstream readFile;
	readFile.open(path.c_str(), ios::binary | ios::in);

	// lỗi: không thể mở file 
	if (!readFile.is_open()) {
		cout << "Duong dan khong hop le" << endl;
		return;
	}

	// tên file
	string filename;
	// Duong dan su dung la /
	if (path.find_last_of('\\') < path.length() && path.find_last_of('\\') >= 0) {
		filename = path.substr(path.find_last_of('\\') + 1);
	}
	else {
		filename = path.substr(path.find_last_of('/') + 1); // xác định tên file từ đường dẫn cung cấp
	}

	// check file name đã tồn tại trong volume hay chưa
	vector<Entry> listEntry = this->readRDET();

	for (int i = 0; i < listEntry.size(); i++) {
		if (filename == listEntry[i].getFileName()) {
			cout << "Ten file da ton tai roi!" << endl;
			return;
		}
	}
	// kích thước tên file
	uint8_t sizeOfFilename = filename.length(); // tính theo bytes
	

	// mật khẩu
	string password = "";
	// kích thước mật khẩu
	uint8_t sizeOfPassword = 0; // mặc định ban đầu không có password

	// tính kích thước file <=> kích thước dữ liệu 
	int begin = readFile.tellg();
	readFile.seekg(0, ios::end);
	int end = readFile.tellg();
	int sizeOfFile = end - begin; // sizeOfFile là kích thước của file cần import (đơn vị byte)
	// vì kích thước volume tối đa là 4GB nên khi sizeOfFile > 4GB = 4,294,967,296 sẽ không import 
	if (sizeOfFile > 4294967296) {
		cout << "Kich thuoc file qua lon de import!" << endl;
		return;
	}
	//Tính số cluster file chiếm
	int clusterOfFileData = ceil((double)sizeOfFile / (512 * sectorPerCluster));
	//Loại FIle
	int type = 0;

	// tính kích thước entry của tệp tin này
	uint16_t sizeOfEntry = 1 + 1 + 1 + 2 + 2 + 1 + 2 + 4 + sizeOfFilename + sizeOfPassword; // N
	// số cluster mà Entry chiếm
	uint16_t amountOfCluster = sizeOfEntry / bytePerSector + 1;  // M

	// Bước 3: đọc bảng FAT vào bộ nhớ
	unsigned int* FAT_table = this->readFat();

	// Bước 4:
	int clusterEntry; // cluster của Entry
	int offsetEntry; // offset của Entry trên cluster đó

	// truy xuất RDET
	vector<int> RDET_cluster;
	RDET_cluster.push_back(rdetCluster);
	
	while (1) {
		if (FAT_table[RDET_cluster[RDET_cluster.size() - 1]] == fileEnd) {
			break;
		}

		RDET_cluster.push_back(FAT_table[RDET_cluster[RDET_cluster.size() - 1]]);
	}

	char* temp = NULL;
	int countByteEmpty = 0;
	bool flag = false;

	int i = 0, j = 0;
	while (i < RDET_cluster.size() ) {
		temp = readCluster(RDET_cluster[i]);
		while (j < sectorPerCluster * 512) {
			char val = *(char*)(&temp[j]);
			if (val == 0) {
				countByteEmpty++;
				if (countByteEmpty == sizeOfEntry) {
					flag = true;
					break;
				}
				j++;
			}
			else {
				j = j + val;
			}
		}
		delete[] temp;

		if (!flag) {
			i++;
		}
		else {
			break;
		}
	}

	// không đủ N byte liên tiếp
	if (!flag) {
		//Tìm cluster trống
		int indexClusterEmpty = -1;
		for (int i = 3; i < numberOfCluters+3; i++) {
			if (FAT_table[i] == 0) {
				indexClusterEmpty = i;
				break;
			}
		}

		// mở rộng bảng RDET
		if (indexClusterEmpty == -1) {
			cout << "Khong du kich thuoc de luu file" << endl;
		}
		else {
			FAT_table[RDET_cluster[RDET_cluster.size() - 1]] = indexClusterEmpty;
			FAT_table[indexClusterEmpty] = fileEnd;

			clusterEntry = indexClusterEmpty;
			offsetEntry = 0;
		}
	}
	else {
		clusterEntry = RDET_cluster[i];
		offsetEntry = j - sizeOfEntry + 1;
	}


	//Tìm cluster trống để lưu dữ liệu
	vector<unsigned int> Data_cluster;
	for (int i = 3; i < numberOfCluters; i++) {
		if (FAT_table[i] == 0) {
			Data_cluster.push_back(i);
			if (Data_cluster.size() == clusterOfFileData) {
				break;
			}
		}
	}

	//TH ko du cluster
	if (Data_cluster.size() < clusterOfFileData) {
		cout << "Khong du khoang trong" << endl;
		return;
	}

	//Điều chỉnh lại bảng FAT
	for (int i = 0; i < Data_cluster.size() - 1; i++) {
		FAT_table[Data_cluster[i]] = Data_cluster[i + 1];
	}
	FAT_table[Data_cluster[Data_cluster.size() - 1]] = fileEnd;

	//Ghi entry vào RDET
	Entry e;
	e.createEntry(filename, password, sizeOfFile, type, Data_cluster[0]); // Khoi tao Entry từ các dữ liệu đã lấy
	char* cluster = readCluster(clusterEntry);
	writeOffset(cluster, offsetEntry, e.toBytes(), sizeOfEntry);
	writeCluster(clusterEntry, cluster);
	delete [] cluster;

	//Ghi lại bảng FAT vào vol
	writeFat(FAT_table);
	
	//Ghi dữ liệu file vào cluster
	readFile.seekg(0);
	for (int i = 0; i < Data_cluster.size(); i++) {
		//Lấy dữ liệu file tương đương số byte 1 cluster
		readFile.seekg(i * 512 * sectorPerCluster, ios_base::beg);
		char* data = new char[512*sectorPerCluster];
		readFile.read(data, 512 * sectorPerCluster);
		
		//Ghi vào cluster của vol
		writeCluster(Data_cluster[i], data);

		delete [] data;
	}

	readFile.close();
}

bool Volume::outport(string filename, vector<Entry*>& listEntry, string outportPath) {
	bool found = false;
	Entry* exportEntry = nullptr;
	int size = exportEntry->getDataSize();

	for (int i = 0; i < listEntry.size(); i++) {
		if (filename == listEntry[i]->getFileName()) {// ktra file co ton tai bang cach ktra ten trong list ten
			found = true;
			exportEntry = listEntry[i];
			break;
		}
	}

	if (!found) return false;

	string password;
	cout << "Nhap mat khau file/folder can export: ";
	getline(cin, password);
	if (!exportEntry->checkPassword(password))
		return false;

	// Neu la file
	FILE* target = nullptr;
	errno_t err = fopen_s(&target, outportPath.c_str(), "wb+");

	unsigned int* FAT_table = this->readFat();

	vector<int> cluster_list; // luu vi tri cac cluster data
	cluster_list.push_back(FAT_table[exportEntry->getStartCluster()]);
	while (cluster_list.back() != fileEnd) { // khac ket thuc file
		cluster_list.push_back(FAT_table[cluster_list.back()]);
	}
	cluster_list.push_back(FAT_table[cluster_list.back()]);


	for (int i = 0; i < cluster_list.size(); ++i) {
		char* buffer = new char[BUFFER_SIZE];
		buffer = readCluster(cluster_list[i]);
		fwrite(buffer, 1, BUFFER_SIZE, target);
		delete[] buffer;
	}

	fclose(target);

	// Neu la folder: chua xu ly

	return true;
}

bool Volume::deleteFile(string filename, vector<Entry*>& listEntry) {
	bool found = false;
	Entry* deleteEntry = nullptr;
	int size = deleteEntry->getDataSize();

	for (int i = 0; i < listEntry.size(); i++) {
		if (filename == listEntry[i]->getFileName()) {// ktra file co ton tai bang cach ktra ten trong list ten
			found = true;
			deleteEntry = listEntry[i];
			break;
		}
	}

	if (!found) return false;

	string password;
	cout << "Nhap mat khau file/folder can export: ";
	getline(cin, password);
	if (!deleteEntry->checkPassword(password))
		return false;

	unsigned int* FAT_table = this->readFat();

	vector<int> cluster_list; // luu vi tri cac cluster data
	cluster_list.push_back(FAT_table[deleteEntry->getStartCluster()]);
	while (cluster_list.back() != 4294967295) { // khac FFFFFFFF
		cluster_list.push_back(FAT_table[cluster_list.back()]);
	}
	cluster_list.push_back(FAT_table[cluster_list.back()]);

	// Cap nhat lai bang FAT
	for (int i = 0; i < cluster_list.size(); ++i) {
		FAT_table[cluster_list[i]] = 1;
	}

	// Cap nhat lai RDET

	// Cap nhat lai Volume

}

vector<Entry> Volume::readRDET() {
	vector<Entry> rdet;

	//đọc bảng FAT
	unsigned int* FAT_table = readFat();
	//Lấy ds cluster cũa RDET
	vector<int> RDET_cluster;
	RDET_cluster.push_back(rdetCluster);
	while (1) {
		if (FAT_table[RDET_cluster[RDET_cluster.size() - 1]] == fileEnd) {
			break;
		}

		RDET_cluster.push_back(FAT_table[RDET_cluster[RDET_cluster.size() - 1]]);
	}

	for (int i = 0; i < RDET_cluster.size(); i++) {
		char* clusterData = readCluster(RDET_cluster[i]);
		int j = 0;
		while (j < 512 * sectorPerCluster) {
			char val = *(char*)(&clusterData[j]);
			if (val == 0) {
				j++;
			}
			else {
				Entry entry;
				entry.readEntry(&clusterData[j]);
				rdet.push_back(entry);
				j += val;
			}
		}
		delete[] clusterData;
	}

	delete[] FAT_table;

	return rdet;
}

void Volume::close() {
	data.close();
}