# 2024 华为软件精英挑战赛 / CodeCraft 2024

<span style="color:grey">Records of the contest By</span> <span style="color:pink">`PinkEx`</span> <span style="color:grey">of team *小羊咩咩*</span>

Ranked 19th in preliminary and 18th in intermediary.

See `quick_view_version/` for executable example. (其他文件夹下是比赛的scripts, 未经整理会比较乱)

**Summary:**

- 初赛 (preliminary): 从泊位和新增物品开始做反向单源BFS做路径规划，非预定制取货/送货，以距离/性价比为关键字排序，实现多目标优先贪心策略 (*multi-target prioritization*) ，通过多目标累计价值函数计算机器人行动 (L/R/U/D/O) 的价值，并以id为优先级实现 "低避高" 避障；调度船舶采用简单贪心，尽可能多地在泊位之间运输，以防某些泊位有大批量剩余物品。此外还有一些边角料优化。面向地图编程调参。

- 复赛 (intermediary): 机器人路径规划策略基本不变，船舶路径规划通过从泊位 (B/K) 和终止点 (T) 反向多源SPFA；需要注意的是，顺/逆时针旋转作用在船舶上并不是可逆的，需要去分别考虑顺/逆时针旋转的逆变换对船舶各个部位位移的影响。正式赛中，考虑到我们的机器人策略做的粒度较粗，因此只购买0-type机器人“降低风险”。船的取货送货通过预定制，需要避障时，只考虑直行/右转。此外，面向地图编程调参，终止点和泊位之间较远时，考虑用多船。

**Problems:**

- 初赛的一大问题在于受种子影响较大，需要进行数据分析。通过batch files和plot modules实现高效率编程和优化实践。
- 复赛的问题 (对我们而言) 在于机器人策略相对于进入复赛的其他队伍比较差，以及无法实现精细控制以更好地运用1-type机器人。
- ……

另外，感谢我的小队友<span style="color:skyblue">`yxf203`</span>陪我打比赛陪我玩！//////