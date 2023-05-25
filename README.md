## [Discord](https://discord.gg/UjgtR7Nqbx)

### 元素系统 (HIGH)  
- 目标:
  - &#10004; 基本的元素单位，目前有：火，水，雷，冰，毒，油  
  - [ ] 元素特性
     1. 火：添加火属性，并附加燃烧效果（每回合扣血）
     2. 水：添加水属性，并附加湿润效果
     3. 雷：添加雷属性，并附加麻痹效果
     4. 冰：添加冰属性，并附加冰冻效果 (无法行动)
     5. 毒：添加毒属性，并附加中毒效果（每回合扣血）
     6. 油：添加油属性，并附加减速效果
  - [ ] 元素单位之间的关系(初阶)
     1. 火
        1. 火 + 水 -> 移除目标身上的火，水属性 //在单位格上生成水蒸气（效果：降低位于范围内单位的可视范围，持续？回合）
        2. 火 + 雷 -> 无效果
        3. 火 + 冰 -> 移除目标身上的火属性，在单位格上生成融化的水（效果：给与范围内目标水附着）
        4. 火 + 毒 -> 触发爆炸（对以当前位置为圆心，半径为1的范围内造成一个火焰伤害），移除火，毒属性
        5. 火 + 油 -> 触发爆炸（对以当前位置为圆心，半径为1的范围内造成一个火焰伤害），移除火，油属性
     2. 水
        1. 水 + 火 -> 移除目标身上的水，火属性，生成蒸汽云
        2. 水 + 雷 -> 为所有相连的水单位格子附加雷属性，概率添加麻痹效果
        3. 水 + 冰 -> 添加冻结效果
        4. 水 + 毒 -> 移除目标身上的水，毒属性
        5. 水 + 油 -> 移除目标身上的水，油属性
     3. 雷
        1. 雷 + 火 -> 无效果
        2. 雷 + 水 -> 为所有相连的水单位格子附加雷属性，概率添加麻痹效果
        3. 雷 + 冰 -> 无效果
        4. 雷 + 毒 -> 无效果
        5. 雷 + 油 -> 无效果
     4. 冰
        1. 冰 + 火 -> 移除目标身上的冰，火属性
        2. 冰 + 水 -> 添加冻结效果
        3. 冰 + 雷 -> 无效果
        4. 冰 + 毒 -> 移除毒属性，添加冰属性
        5. 冰 + 油 -> 移除油属性，添加冰属性
     5. 毒
        1. 毒 + 火 -> 触发爆炸（对以当前位置为圆心，半径为1的范围内造成一个火焰伤害，如果范围内存在其他油毒属性地面，那么会再次出发爆炸），移除火，毒属性，所有相连的
        2. 毒 + 水 -> 移除毒属性，添加水属性
        3. 毒 + 雷 -> 无效果
        4. 毒 + 冰 -> 移除毒属性，添加冰属性
        5. 毒 + 油 -> 移除毒属性，添加油属性
     6. 油
        1. 油 + 火 -> 触发爆炸（对以当前位置为圆心，半径为1的范围内造成一个火焰伤害，如果范围内存在其他油毒属性地面，那么会再次出发爆炸），移除火，油属性
        2. 油 + 水 -> 移除油属性，添加水属性
        3. 油 + 雷 -> 无效果
        4. 油 + 冰 -> 移除油属性，添加冰属性
        5. 油 + 毒 -> 移除油属性，添加毒属性
  - 高阶元素反应
    - 水 + 雷 + 火 -> 移除水属性，添加雷电云
  - [ ] 表现形式
     1. 火：在棋子身上显示燃烧效果（待定），添加 buff 图标；在地面显示燃烧 decal
     2. 水：在棋子身上显示湿润效果（待定），添加 buff 图标；在地面显示水蒸气 decal
     3. 雷：在棋子身上显示麻痹效果（待定），添加 buff 图标；在地面显示雷电 decal
     4. 冰：在棋子身上显示冻结效果（待定），添加 buff 图标；在地面显示冰块 decal
     5. 毒：在棋子身上显示中毒效果（待定），添加 buff 图标；在地面显示毒液 decal
     6. 油：在棋子身上显示减速效果（待定），添加 buff 图标；在地面显示油污 decal
- TODO
  思路：
  - &#10004; 当有单位进入到该Tile中，我们遍历该Tile上所有激活的GameplayEffect, 这个GameplayEffect应该有一个特定类型（GridGameplayEffect_GridMapNode），里面保存了一组进入时给与的GameplayEffect，如果遍历到的一个GE是该类型，那么就对进入Tile的对象施加这个GE
  - &#10004; 每回合都会对当前位于Tile中的对象施加一次
  - &#10004; 相邻位置应用GameplayEffect，对于每个Tile, 查找相邻的Tiles(八方向)，搜索，找到所有相邻的Tiles，然后对每个Tile应用相应的GameplayEffect
  - [x] 实现范围攻击的 TargetType
- 设计方法：
```
GameplayEffect: 引发类型，用来触发实际将会应用的GE
目标类型 (ConditionGameplayEffect)
ChessPiece:
	-> 实际的GE
GridMapNode:
	-> 实际的GE
Tips: 不再指定其他属性

GridGameplayEffect_ChessPiece: 可以被应用到棋子上的GE
可以包括实际的属性修改
指定GameplayTag
持续
组合 -> GridGameplayEffect_ChessPiece (组合需要一个新的GameplayEffect)
移除

GridGameplayEffect_GridMapNode: 可以被应用到地面的GE
组合 -> GridGameplayEffect_GridMapNode: 
	-> ConductionGameplayEffects 可被传导的GameplayEffect
因此，一个可被传导的GE需要多个GE的组合才能有效果
1. 本元素GE
2. 组合产生的GE
3. 由于组合产生了一个可传导GE（AttachGameplayEffects）, 需要一个GE来应用到 ChessPiece 上

/// Class

UGridGameplayEffect
-> ConditionGameplayEffect

UGridGameplayEffect_Trigger: UGridGameplayEffect

UGridGameplayEffect_ChessPiece: UGridGameplayEffect

UGridGameplayEffect_GridMapNode: UGridGameplayEffect
-> AttachGameplayEffects
-> ConductionGameplayEffects


/// Tags
Ability.Element.Prue.* 原始的
Ability.Element.Compose.* 组合的


/// Name
GE_Element_Trigger_* 用于触发
GE_Element_ChessPiece_* 将应用于棋子上
GE_Element_GridMapMod_* 将应用于网格上
```

### 寻路

#### 目标：动态场景寻路 (HIGH)
在不改变当前 a* 寻路算法的前提下，实现动态场景寻路
##### 难点：
- [ ] 连续寻路 允许玩家在寻路未结束时多次点击地面，改变寻路终点
   1. 最好能根据玩家点击位置，调整需要寻路的路径，而不是对全图进行搜索
   2. 重新寻路后，玩家开始位置应该是当前的实时位置而不是当前GridMap上格子的位置
- [ ] 动态躲避 障碍物在寻路过程中发生移动，寻路算法需要重新计算     
   1. 每帧去查看下一个Tile是否被占用，如果是，那么重新寻路
- [ ] 群体行动时，很容易出现阵型中某个目标位置不可达，此时需要重新寻路
   1. 对于所有不可达的目标位置，遍历之，以他为圆心，每次增加1单位半径，直到找到一个合适的位置可以被放置
   2. 如果所有的目标位置都不可达，那么就放弃这次行动
   3. 需要显示所有目标位置

#### 目标：战斗范围
说明：  
这里我们需要一个战斗范围体积（类似于volume或者碰撞体），当玩家进入这个范围时，会执行战斗逻辑（状态机，可以是一段剧情或者一场战斗）。当玩家离开这个范围时，会退出战斗逻辑。当战斗逻辑结束时，会销毁这个范围体积。

### 战斗模式
目前是类似文明的形式，感觉效果不太好，想换成神界原罪的形式，以速度为行动顺序
#### 目标：回合制速度条
- [ ] 每个角色有一个速度条，速度条满了之后，角色可以行动，作为属性存储在角色身上，每次行动后速度条清空
- [ ] 因此角色上有两个属性，1：每 CT 增长速度（可以被外部修改）；2：当前速度值（参与实际计算的数值，每回合清空且不可以被外部修改）
- [ ] 需要维护一个行动队列，每回合开始时，将所有角色按照当前速度值从大到小排序，然后依次行动
- [ ] 每回合开始前，都通过一个while(true)循环不停的对他们的速度进行加操作，一直到有一个单位的速度达到可以行动的阈值，这个单位就是本回合可以行动的单位。（或许这里就可以将这个单位从行动队列中移除了）
- [ ] 每回合结束前，都会将当前行动单位的速度值清空，且将其从行动队列头移动到行动队列尾部。
- [ ] 每回合行动时，如果使用了任何加速的GE, 那么将会对行动队列重新进行计算（加一次速度），然后再次排序。
- [ ] 需要有一个UI，显示在画面头部，用于展示行动队列和当前行动单位。

### 特殊宏标记 （需要在源码引擎里面添加）
- [x] ENABLE_TURN_BASE_GAME_ABILITY: 是否启用回合制游戏模式
- [x] ENABLE_CURSOR_VISIBLE_IN_GAME: 是否在游戏中显示鼠标

### 编辑器扩展
#### 目标 &#10004;
为了方便的对UE编辑器扩展以便添加一些自定义功能，需要对UE编辑器进行扩展 
#### 使用方式：
```c++
UCLASS()
class UMyClass : public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(meta=(MenuItem="MyTest/Test", ToolTip="这是一个测试方法"))
    void MyEditorFunction();
};

// 将会在编辑器的菜单栏中添加一个 MyTest 菜单，点击后会执行 MyEditorFunction 方法
```
    
```c++
#include <iostream>
#include <vector>
#include <utility>

std::vector<std::pair<int, int>> GetTileIndexesInRange(const std::vector<std::pair<int, int>>& vecs, int range, int index) {
    std::vector<std::pair<int, int>> new_tmp_vecs;
    int total = (range + range + 1);
    for (int x = 0; x < total; ++x) {
        for (int y = 0; y < total; ++y) {
            std::pair<int, int> new_vec = std::make_pair(x - range, y - range);
            new_tmp_vecs.push_back(new_vec);
        }
    }
    std::vector<std::pair<int, int>> output_vecs;
    for (const auto& vec : new_tmp_vecs) {
        output_vecs.push_back(std::make_pair(vecs[index].first + vec.first, vecs[index].second + vec.second));
    }
    return output_vecs;
}

int main() {
    // 创建一个 10x10 的 vecs
    std::vector<std::pair<int, int>> vecs;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            vecs.push_back(std::make_pair(i, j));
        }
    }

    // 随机选择一个位置
    int index = 25; // rand() % vecs.size();

    // 调用函数并打印结果
    auto result = GetTileIndexesInRange(vecs, 2, index);
   
    // 可视化结果
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            bool found = false;
            for (const auto& tile : result) {
                if (tile.first == i && tile.second == j) {
                    found = true;
                    break;
                }
            }
            if (found) {
                std::cout << "X ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }


    return 0;
}
```



    
