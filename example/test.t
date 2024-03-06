local c = terralib.includec("stdio.h");
terra main()
    var listaint = ListInt.new();
    var listaint2 = ListInt.new();

    var listaintint = ListListInt.new();

    listaintint:push(listaint);
    listaintint:push(listaint2);

    listaint2:push(1);
    listaint2:push(2);
    listaint2:push(3);

    listaint:push(12312);
    listaint:push(2);
    listaint:insert(333321, 0);
    c.printf("[1][2] = %d\n", listaintint:get(1):get(2));
    var str = String.new();
    str:push(('a')[0]);
    str:push(('b')[0]);
    str:push(('c')[0]);
    str:push(('d')[0]);
    str:push(('e')[0]);
    str:push(('f')[0]);
    str:push(('g')[0]);
    str:push(('h')[0]);
    str:push(('i')[0]);
    str:push(('j')[0]);
    str:push(('d')[0]);
    str:push(('e')[0]);
    str:push(('f')[0]);
    str:push(('g')[0]);
    str:push(('h')[0]);
    str:push(('i')[0]);

    var str2 = String.fromString("def");

    var str3:String = String.fromString("xyz");
    
    str:replace(str2, str3);
    
    str:print("str = ");
    str3:print("str3 = ");

    var str4 = String.fromString("y");

    var strsplited = str:split(str4);

    c.printf("strsplited[0] = %s\n", strsplited:get(0).array);
    c.printf("strsplited[1] = %s\n", strsplited:get(1).array);
    c.printf("strsplited[2] = %s\n", strsplited:get(2).array);
end

return main;