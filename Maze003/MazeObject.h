#pragma once

#include <vector>
#include <cstdint>
#include <random>
#include <tuple>

class MazeObject {
public:

	typedef std::uintmax_t SizeType;
	class Tyle {
	public:
		enum WallPosition {
			//NonePos=0,
			TopPos,//Z+
			RightPos,//X+
			BottomPos,//Z-
			LeftPos,//X-
			UpPos,//Y+
			DownPos,//Y-
		};
		enum WallBits {
			NoneBit=0,
			TopBit = 1,
			RightBit=2,
			BottomBit=4,
			LeftBit =8,
			UpBit = 16,
			DownBit = 32,
			All=0x3f,
		};
		bool DropWall(const std::uint8_t& W) {
			Wall |= W;
			return true;
		}
		bool SetWall(const std::uint8_t& W) {
			Wall &= (~W);
			return true;
		}

		bool HitTest(const std::uint8_t& W) const{
			std::uint8_t V =  ((Wall & W));
			return V > 0;
			
			//return ((Wall & W) > 0);
		}

		operator std::uint8_t() {
			return Wall;
		}
	protected:
		std::uint8_t Wall = 0;
	};
public:

	std::uintmax_t Width() const{
		return W;
	}

	std::uintmax_t Height() const{
		return H;
	}

	bool Mining(const std::uintmax_t& Width, const std::uintmax_t& Height) {
		ReSize(Width, Height);
		if (Width == 0) return false;
		if (Height == 0) return false;
		
		std::uniform_int_distribution<std::uintmax_t> RX(0, Width - 1);
		std::uniform_int_distribution<std::uintmax_t> RY(0, Height - 1);
		std::random_device rd;
		std::mt19937 mt;
		
		DoMining(RX(mt), RY(mt));
		//DoMining(0, 0);

		return true;
	}

	Tyle Index(const std::uintmax_t& X, const std::uintmax_t Y)const  {
		if (X >= W) return {};
		if (Y >= H)return {};

		return MF[Y][X];
	}


	bool Clear() {
		MF.clear();
		return true;
	}

protected:
	bool DoMining_r(const std::uintmax_t& X, const std::uintmax_t& Y) {
		std::vector<std::int8_t> XP{ 0,1,0,-1 };
		std::vector<std::int8_t> YP{ 1,0,-1,0 };
		std::vector<std::int8_t> Dirs{ 0,1,2,3 };
		std::vector<std::int8_t> DirsR{ 2,3,0,1 };
		std::random_device rd;

		std::shuffle(Dirs.begin(), Dirs.end(), rd);

		if (X >= W)return false;
		if (Y >= H)return false;

		for (std::size_t i = 0; i < Dirs.size(); i++) {
			std::uintmax_t PX = X + XP[Dirs[i]];
			std::uintmax_t PY = Y + YP[Dirs[i]];
			if (PX >= W) continue;
			if (PY >= H)continue;
			if (MF[PY][PX] != 0)continue;
			MF[Y][X].DropWall(1 << Dirs[i]);
			MF[PY][PX].DropWall(1 << DirsR[Dirs[i]]);

			DoMining_r(PX, PY);
		}

		return true;

	}

	typedef std::tuple < std::uintmax_t, std::uintmax_t, std::vector<std::uint8_t>, std::size_t> StackData;
	typedef std::vector<StackData> Stack;

	bool DoMining(const std::uintmax_t& X, const std::uintmax_t& Y) {
		std::vector<std::int8_t> XP{ 0,1,0,-1 };
		std::vector<std::int8_t> YP{ 1,0,-1,0 };
		std::vector<std::uint8_t> Dirs{ 0,1,2,3 };
		std::vector<std::uint8_t> DirsR{ 2,3,0,1 };
		std::random_device rd;

		Stack S;

		std::uintmax_t XPos = 0;
		std::uintmax_t YPos = 0;
		std::size_t i = 0;

		S.push_back({ X,Y,Dirs,0 });
		while (S.size() != 0) {
			std::tie(XPos, YPos, Dirs, i) = S.back();
			std::vector<std::uint8_t> Dirs2 = Dirs;
			//std::size_t i = std::get<3>(S.back());
			S.pop_back();

			if (XPos >= W)continue;
			if (YPos >= H)continue;
			if (i >= Dirs.size())continue;

			for (; i < Dirs.size(); i++) {
				std::uintmax_t PX = XPos + XP[Dirs[i]];
				std::uintmax_t PY = YPos + YP[Dirs[i]];
				if (PX >= W) continue;
				if (PY >= H)continue;
				if (MF[PY][PX] != 0)continue;
				MF[YPos][XPos].DropWall(1 << Dirs[i]);
				MF[PY][PX].DropWall(1 << DirsR[Dirs[i]]);
				S.push_back({ XPos,YPos,Dirs2,i });	
				std::shuffle(Dirs.begin(), Dirs.end(), rd);
				S.push_back({ PX,PY,Dirs,0 });				
			}
		}
		return true;
	}
	bool ReSize(const std::uintmax_t& Width, const std::uintmax_t& Height) {
		MF.clear();

		MF.resize(Height);

		for (auto& o : MF) {
			o.resize(Width);
		}
		W = Width;
		H = Height;

		return true;
	}
protected:
	typedef std::vector<std::vector<Tyle>> MazeField;

	MazeField MF;
	std::uintmax_t W = 0;
	std::uintmax_t H = 0;
};

bool ShowMaze(std::ostream& os, MazeObject& MO, char Wall = 'W', char Empty = ' ') {//[name.txt]

	for (MazeObject::SizeType Y = 0; Y < MO.Height(); Y++) {
		for (MazeObject::SizeType X = 0; X < MO.Width(); X++) {
			os << Wall;
			os << (MO.Index(X, Y).HitTest(1 << MazeObject::Tyle::BottomPos) ? Empty : Wall);
			os << Wall;
		}
		os << std::endl;
		for (MazeObject::SizeType X = 0; X < MO.Width(); X++) {
			os << (MO.Index(X, Y).HitTest(1 << MazeObject::Tyle::LeftPos) ? Empty : Wall);
			os << Empty;
			os << (MO.Index(X, Y).HitTest(1 << MazeObject::Tyle::RightPos) ? Empty : Wall);

		}
		os << std::endl;
		for (MazeObject::SizeType X = 0; X < MO.Width(); X++) {
			os << Wall;
			os << (MO.Index(X, Y).HitTest(1 << MazeObject::Tyle::TopPos) ? Empty : Wall);
			os << Wall;
		}
		os << std::endl;
	}
	return true;
}

bool MazeToPBM(std::ostream& os, const MazeObject& MO, char Wall = '1', char Empty = '0') {//[name.pbm]

	os << "P1" << std::endl;

	os << "# This is Maze Image(" << MO.Width() << 'x' << MO.Height() << ")." << std::endl;

	os << MO.Width() * 3 << ' ' << MO.Height() * 3 << std::endl;

	for (MazeObject::SizeType Y = 0; Y < MO.Height(); Y++) {
		for (MazeObject::SizeType X = 0; X < MO.Width(); X++) {
			os << Wall;
			os << (MO.Index(X, Y).HitTest(1 << MazeObject::Tyle::BottomPos) ? Empty : Wall);
			os << Wall;
		}
		os << std::endl;
		for (MazeObject::SizeType X = 0; X < MO.Width(); X++) {
			os << (MO.Index(X, Y).HitTest(1 << MazeObject::Tyle::LeftPos) ? Empty : Wall);
			os << Empty;
			os << (MO.Index(X, Y).HitTest(1 << MazeObject::Tyle::RightPos) ? Empty : Wall);

		}
		os << std::endl;
		for (MazeObject::SizeType X = 0; X < MO.Width(); X++) {
			os << Wall;
			os << (MO.Index(X, Y).HitTest(1 << MazeObject::Tyle::TopPos) ? Empty : Wall);
			os << Wall;
		}
		os << std::endl;
	}
	return true;
}