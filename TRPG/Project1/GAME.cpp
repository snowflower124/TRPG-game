#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

// 유닛 구조체
struct Unit {
    string name;
    int hp, maxHP;
    int mp, maxMP;
    int atk;
    bool buff = false;
    bool debuff = false;
};

// 일반 랜덤
int r(int a, int b) { return rand() % (b - a + 1) + a; }

// ★ 10 단위 랜덤 생성 (예: 90~120 → 90,100,110,120)
int r10(int a, int b) {
    int step = ((b - a) / 10) + 1;
    return a + (rand() % step) * 10;
}

// 데미지 계산
int calcDamage(Unit& u) {
    int dmg = u.atk;
    if (u.buff) dmg += 5;
    if (u.debuff) dmg -= 5;
    return max(1, dmg);
}

int main() {
    srand(time(0));

    // =======================
    //     캐릭터 생성
    // =======================

    // ★ 플레이어 : HP 90~120 / MP 30~50
    Unit player = {
        "Player",
        r10(90, 120), r10(90, 120),
        r10(30, 50),  r10(30, 50),
        15
    };

    // ★ 아군 랜덤 생성
    int allyCount = r(1, 4);
    vector<Unit> allies;

    for (int i = 0; i < allyCount; i++) {
        allies.push_back({
            "Ally" + to_string(i + 1),
            r10(90, 120), r10(90, 120),
            r10(30, 50),  r10(30, 50),
            r(10, 15)
            });
    }

    // ★ 적 랜덤 생성
    int enemyCount = r(1, 4);
    vector<Unit> enemies;

    for (int i = 0; i < enemyCount; i++) {
        enemies.push_back({
            "Enemy" + to_string(i + 1),
            r10(90, 120), r10(90, 120),
            r10(30, 50),  r10(30, 50),
            r(8, 14)
            });
    }

    cout << "=== 랜덤 파티 RPG 전투 시작! ===\n";
    cout << "동료 " << allyCount << "명, 적 " << enemyCount << "명\n\n";

    // =======================
    //       전투 루프
    // =======================
    while (true) {

        // -----------------------
        //    승패 판정
        // -----------------------
        bool enemiesDead = true;
        for (auto& e : enemies)
            if (e.hp > 0) enemiesDead = false;

        bool alliesDead = (player.hp <= 0);
        for (auto& a : allies)
            if (a.hp > 0) alliesDead = false;

        if (enemiesDead) {
            cout << "\n🎉 승리! 모든 적을 해치웠습니다.\n";
            break;
        }

        if (alliesDead) {
            cout << "\n💀 패배… 모든 아군이 쓰러졌습니다.\n";
            break;
        }

        // -----------------------
        //     상태 출력
        // -----------------------
        cout << "\n--- 아군 상태 ---\n";
        cout << player.name << " HP:" << player.hp << "/" << player.maxHP
            << " MP:" << player.mp << "/" << player.maxMP << "\n";

        for (auto& a : allies)
            cout << a.name << " HP:" << a.hp << " MP:" << a.mp << "\n";

        cout << "\n--- 적 상태 ---\n";
        for (auto& e : enemies)
            cout << e.name << " HP:" << e.hp << "\n";


        // -----------------------
        //     플레이어 턴
        // -----------------------
        cout << "\n--- 플레이어 턴 ---\n";
        cout << "1. 공격  2. 강공격(10MP)  3. 회복(8MP)  4. 방어\n선택: ";

        int choice;
        cin >> choice;

        int target = -1;

        if (choice != 4) {
            cout << "공격할 적 번호 (1~" << enemyCount << "): ";
            cin >> target;
            target--;

            if (target < 0 || target >= enemyCount || enemies[target].hp <= 0) {
                cout << "잘못된 선택! 턴을 스킵합니다.\n";
                goto END_TURN;
            }
        }

        // 행동 처리
        if (choice == 1) {
            int dmg = calcDamage(player);
            enemies[target].hp -= dmg;
            cout << "플레이어 공격! " << dmg << " 피해!\n";
        }
        else if (choice == 2) {
            if (player.mp < 10) {
                cout << "MP 부족!\n";
                goto END_TURN;
            }
            player.mp -= 10;
            int dmg = calcDamage(player) + 10;
            enemies[target].hp -= dmg;
            cout << "강공격! " << dmg << " 피해!\n";
        }
        else if (choice == 3) {
            if (player.mp < 8) {
                cout << "MP 부족!\n";
                goto END_TURN;
            }
            player.mp -= 8;
            int heal = r(20, 35);
            player.hp = min(player.maxHP, player.hp + heal);
            cout << "회복 스킬! +" << heal << " HP\n";
        }
        else if (choice == 4) {
            cout << "플레이어가 방어 자세를 취합니다!\n";
            player.buff = true;
        }

        // -----------------------
        //     아군 자동 공격
        // -----------------------
        for (auto& a : allies) {
            if (a.hp <= 0) continue;

            int idx = r(0, enemyCount - 1);
            if (enemies[idx].hp <= 0) continue;

            int dmg = calcDamage(a);
            enemies[idx].hp -= dmg;

            cout << a.name << " → " << enemies[idx].name
                << "에게 " << dmg << " 피해!\n";
        }

        // -----------------------
        //   랜덤 버프 / 디버프
        // -----------------------
        if (r(1, 100) <= 20) {
            if (r(0, 1)) {
                cout << "\n✨ 버프 발생! 플레이어 공격력 증가!\n";
                player.buff = true;
            }
            else {
                cout << "\n💀 디버프 발생! 플레이어 공격력 감소!\n";
                player.debuff = true;
            }
        }

        // -----------------------
        //        적 턴
        // -----------------------
        cout << "\n--- 적 턴 ---\n";

        for (auto& e : enemies) {
            if (e.hp <= 0) continue;

            int total = allies.size() + 1;
            int idx = r(0, total - 1);

            Unit* targetUnit = (idx == 0 ? &player : &allies[idx - 1]);

            int dmg = calcDamage(e);
            targetUnit->hp -= dmg;

            cout << e.name << " → " << targetUnit->name
                << "에게 " << dmg << " 피해!\n";
        }

        // -----------------------
        //     턴 종료 구분선
        // -----------------------
    END_TURN:
        cout << "\n============================== 턴 종료 ==============================\n";
    }

    return 0;
}