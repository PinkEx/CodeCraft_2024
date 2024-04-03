/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

package com.huawei.codecraft;

import java.util.Random;
import java.util.Scanner;
import java.util.List;
import java.util.ArrayList;

/**
 * Main
 *
 * @since 2024-02-05
 */
public class Main {
    private static final int N = 200;
    private static int robot_num = 0, berth_num = 0, boat_num = 0;

    private static int money, id, boat_capacity, boat_price=8000, robot_price=2000;
    private String[] ch = new String[N];

    private Robot[] robot = new Robot[20];
    private Berth[] berth = new Berth[10];
    private Boat[] boat = new Boat[10];

    static List<int[]> robot_purchase_point = new ArrayList<>();
    static List<int[]> boat_purchase_point = new ArrayList<>();
    static List<int[]> delivery_point = new ArrayList<>();

    private void ProcessMap() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (ch[i].charAt(j) == 'R')
                   robot_purchase_point.add(new int[]{i, j});
                else if (ch[i].charAt(j) == 'S')
                   boat_purchase_point.add(new int[]{i, j});
                else if (ch[i].charAt(j) == 'T')
                   delivery_point.add(new int[]{i, j});
            }
        }
    }

    private void init() {
        Scanner scanf = new Scanner(System.in);
        for(int i = 0; i < N; i++) {
            ch[i] = scanf.nextLine();
        }

	ProcessMap();
        berth_num = scanf.nextInt();
        for (int i = 0; i < berth_num; i++) {
            int id = scanf.nextInt();
            berth[id] = new Berth();
            berth[id].x = scanf.nextInt();
            berth[id].y = scanf.nextInt();
            berth[id].loading_speed = scanf.nextInt();
        }
        this.boat_capacity = scanf.nextInt();
        String okk = scanf.nextLine();
        System.out.println("OK");
        System.out.flush();
    }

    private int input() {
        Scanner scanf = new Scanner(System.in);
	this.id = scanf.nextInt();
        this.money = scanf.nextInt();
        int num = scanf.nextInt();
        for (int i = 1; i <= num; i++) {
            int x = scanf.nextInt();
            int y = scanf.nextInt();
            int val = scanf.nextInt();
        }
        robot_num = scanf.nextInt();
        for(int i = 0; i < robot_num; i++) {
            robot[i] = new Robot();
            robot[i].id = scanf.nextInt();
            robot[i].goods = scanf.nextInt();
            robot[i].x = scanf.nextInt();
            robot[i].y = scanf.nextInt();
        }
	boat_num = scanf.nextInt();
        for(int i = 0; i < boat_num; i ++) {
            boat[i] = new Boat();
            boat[i].id = scanf.nextInt();
            boat[i].num = scanf.nextInt();
            boat[i].x = scanf.nextInt();
            boat[i].y = scanf.nextInt();
            boat[i].status = scanf.nextInt();
        }
        String okk = scanf.nextLine();
        return id;
    }

    public static void main(String[] args) {
        Main mainInstance = new Main();
        mainInstance.init();
        for(int zhen = 1; zhen <= 15000; zhen ++) {
            int id = mainInstance.input();
            Random rand = new Random();
            if(money >= robot_price && robot_num <= 1){
                System.out.printf("lbot %d %d\n", robot_purchase_point.get(0)[0], robot_purchase_point.get(0)[1]);
            }

            if(money >= boat_price && boat_num <= 1){
                System.out.printf("lboat %d %d\n", boat_purchase_point.get(0)[0], boat_purchase_point.get(0)[1]);
            }
            for(int i = 0; i < robot_num; i ++) {
                System.out.printf("move %d %d\n", i, rand.nextInt(4) % 4);
            }
            for(int i = 0; i < boat_num; i ++) {
                int status = Math.abs(rand.nextInt()) % 2;
                if(status == 1)
                  System.out.printf("ship %d\n", i);
                else
                 System.out.printf("rot %d %d \n", i, rand.nextInt() % 2);
        }
            System.out.println("OK");
            System.out.flush();
        }
    }

    class Robot {
        int id, x, y, goods;

        public Robot() {}

        public Robot(int startX, int startY) {
            this.x = startX;
            this.y = startY;
        }
    }

    class Berth {
        int x;
        int y;
        int loading_speed;
        public Berth(){}
        public Berth(int x, int y, int transport_time, int loading_speed) {
            this.x = x;
            this.y = y;
            this.loading_speed = loading_speed;
        }
    }

    class Boat {
        int id, x, y, dir;
        int num;
        int status;
    }

}
