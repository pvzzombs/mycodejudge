create table problems (id integer, title text, description text, testcases text, answers text);
create table users (id integer, username text, password text);
create table sessions (username text, sessionid text);
create table admin (username text, password text);
create table solutions (id integer, title text, submitdate text, solution text, username text, isSolved text);
