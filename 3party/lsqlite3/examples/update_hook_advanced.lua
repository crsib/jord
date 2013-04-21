
local sqlite3 = require("lsqlite3")
local db

optbl = { [sqlite3.UPDATE] = "UPDATE";
          [sqlite3.INSERT] = "INSERT";
          [sqlite3.DELETE] = "DELETE"
        }
setmetatable(optbl,
	{__index=function(t,n) return string.format("Unknown op %d",n) end})


function update_hook(ud, op, dname, tname, rowid)
  print("Sqlite Update Hook:", optbl[op], dname, tname, rowid)
end

local tests = {}

-- runs specified test with all necessary setup
-- @param string name
function run_test(name)
  -- banner
    print(string.format([[


  ==========================
   %s
  ==========================
  ]], name))

  -- setup
  db = sqlite3.open_memory()
  local udtbl = {0, 0, 0}
  db:update_hook(update_hook, udtbl)

  -- run test
  tests[name]()

  -- destroy
  db:close()
end

function tests.insert_select()
  db:exec[[
    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY, content VARCHAR );

    INSERT INTO T1 VALUES (NULL, 'Hello World');
    INSERT INTO T1 VALUES (NULL, 'Hello Lua');
    INSERT INTO T1 VALUES (NULL, 'Hello Sqlite3');

    INSERT INTO T2 SELECT * FROM T1;
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end
end

function tests.trigger_insert()
  db:exec[[
    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY, content VARCHAR );

    CREATE TRIGGER after_insert_T1
    AFTER INSERT ON T1
    FOR EACH ROW
    BEGIN
      INSERT INTO T2 VALUES(NEW.id, NEW.content);
    END;

    INSERT INTO T1 VALUES (NULL, 'Hello World');
    INSERT INTO T1 VALUES (NULL, 'Hello Lua');
    INSERT INTO T1 VALUES (NULL, 'Hello Sqlite3');
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end
end

function tests.cascade_delete()
  db:exec[[
    PRAGMA foreign_keys = ON;

    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY REFERENCES T1 ON DELETE CASCADE, content VARCHAR );
    CREATE TABLE T3 ( id INTEGER PRIMARY KEY REFERENCES T2 ON DELETE CASCADE, content VARCHAR );

    INSERT INTO T1 VALUES (NULL, 'a');
    INSERT INTO T1 VALUES (NULL, 'b');
    INSERT INTO T1 VALUES (NULL, 'c');
    INSERT INTO T1 VALUES (NULL, 'd');

    INSERT INTO T2 SELECT * FROM T1;
    INSERT INTO T3 SELECT * FROM T2;

    DELETE FROM T1 WHERE id < 3;
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end

  print("\nT3 contents:")
  for row in db:nrows("SELECT * FROM T3") do
    print(row.id, row.content)
  end
end

function tests.cascade_update_update()
  db:exec[[
    PRAGMA foreign_keys = ON;

    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY REFERENCES T1 ON UPDATE CASCADE, content VARCHAR );
    CREATE TABLE T3 ( id INTEGER PRIMARY KEY REFERENCES T2 ON UPDATE CASCADE, content VARCHAR );

    INSERT INTO T1 VALUES (NULL, 'a');
    INSERT INTO T1 VALUES (NULL, 'b');
    INSERT INTO T1 VALUES (NULL, 'c');
    INSERT INTO T1 VALUES (NULL, 'd');

    INSERT INTO T2 SELECT * FROM T1;
    INSERT INTO T3 SELECT * FROM T2;

    UPDATE T1 SET id = id + 10 WHERE id < 3;
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end

  print("\nT3 contents:")
  for row in db:nrows("SELECT * FROM T3") do
    print(row.id, row.content)
  end
end

-- hook "anomaly"
-- implicit one-statement transaction rollback demonstration
function tests.cascade_update_restrict()
  db:exec[[
    PRAGMA foreign_keys = ON;

    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY REFERENCES T1 ON UPDATE RESTRICT, content VARCHAR );
    CREATE TABLE T3 ( id INTEGER PRIMARY KEY REFERENCES T2 ON UPDATE RESTRICT, content VARCHAR );

    INSERT INTO T1 VALUES (NULL, 'a');
    INSERT INTO T1 VALUES (NULL, 'b');
    INSERT INTO T1 VALUES (NULL, 'c');
    INSERT INTO T1 VALUES (NULL, 'd');

    INSERT INTO T2 SELECT * FROM T1;
    INSERT INTO T3 SELECT * FROM T2;

    -- this update gets reverted but the update_hook with rowid=11 *DOES* get triggered
    UPDATE T1 SET id = id + 10 WHERE id < 3;
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end

  print("\nT3 contents:")
  for row in db:nrows("SELECT * FROM T3") do
    print(row.id, row.content)
  end
end

-- hook "anomaly"
-- case is analogous to cascade_update_restrict
function tests.cascade_delete_restrict()
  db:exec[[
    PRAGMA foreign_keys = ON;

    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY REFERENCES T1 ON DELETE RESTRICT, content VARCHAR );
    CREATE TABLE T3 ( id INTEGER PRIMARY KEY REFERENCES T2 ON DELETE RESTRICT, content VARCHAR );

    INSERT INTO T1 VALUES (NULL, 'a');
    INSERT INTO T1 VALUES (NULL, 'b');
    INSERT INTO T1 VALUES (NULL, 'c');
    INSERT INTO T1 VALUES (NULL, 'd');

    INSERT INTO T2 SELECT * FROM T1;
    INSERT INTO T3 SELECT * FROM T2;

    DELETE FROM T1 WHERE id < 3;
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end

  print("\nT3 contents:")
  for row in db:nrows("SELECT * FROM T3") do
    print(row.id, row.content)
  end
end

-- no anomalies here
function tests.fk_violate_insert()
  db:exec[[
    PRAGMA foreign_keys = ON;

    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY REFERENCES T1, content VARCHAR);

    INSERT INTO T1 VALUES (NULL, 'a');
    INSERT INTO T1 VALUES (NULL, 'b');

    INSERT INTO T2 VALUES(99, 'xxx');
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end
end

-- hook "anomaly"
function tests.fk_violate_update()
  db:exec[[
    PRAGMA foreign_keys = ON;

    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY REFERENCES T1, content VARCHAR);

    INSERT INTO T1 VALUES (NULL, 'a');
    INSERT INTO T1 VALUES (NULL, 'b');
    INSERT INTO T2 VALUES(1, 'a');

    -- update doesn't succeed but we get a hook for tuple with rowid=99 in T2
    UPDATE T2 SET id = 99 WHERE id = 1;
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end
end

function tests.cascade_update_setnull()
  db:exec[[
    PRAGMA foreign_keys = ON;

    CREATE TABLE T1 ( id INTEGER PRIMARY KEY, content VARCHAR );
    CREATE TABLE T2 ( id INTEGER PRIMARY KEY, content INTEGER REFERENCES T1(id) ON UPDATE SET NULL);

    INSERT INTO T1 VALUES (NULL, 'a');
    INSERT INTO T1 VALUES (NULL, 'b');
    INSERT INTO T1 VALUES (NULL, 'c');
    INSERT INTO T1 VALUES (NULL, 'd');

    INSERT INTO T2 SELECT NULL, id FROM T1;

    UPDATE T1 SET id = id + 10 WHERE id < 3;
  ]]

  print("\nT1 contents:")
  for row in db:nrows("SELECT * FROM T1") do
    print(row.id, row.content)
  end

  print("\nT2 contents:")
  for row in db:nrows("SELECT * FROM T2") do
    print(row.id, row.content)
  end
end


-- run_test('fk_violate_update')

for k,v in pairs(tests) do
    run_test(k)
end
