import re, sys

STR = r'"(?:[^"\\]|\\.)*"'
TOK = re.compile(rf'\s*(?:({STR})|(\w+)|([{{}},]))')

def parse_table(src):

    m = re.search(r'\bclOptions\s*(?:=\s*)?\{', src)
    if not m:
        print("  !! could not find a 'clOptions = {' definition")
        return []
    i = m.end() - 1

    # i = src.index('{', src.index('clOptions'))
    rows, depth, fields, cur = [], 0, [], None
    for m in TOK.finditer(src, i):
        s, ident, p = m.groups()
        if p == '{':
            depth += 1
            if depth == 2:
                fields, cur = [], None
        elif p == '}':
            if depth == 2:
                if cur is not None:
                    fields.append(cur)
                rows.append(fields)
            depth -= 1
            if depth == 0:
                break
        elif p == ',' and depth == 2:
            fields.append(cur)
            cur = None
        elif s is not None and depth == 2:
            cur = (cur or '') + s[1:-1]        # adjacent literals concatenate
        elif ident and depth == 2:
            cur = ident                         # true / false
    out = []
    for f in rows:
        p = f + [None] * (5 - len(f))
        out.append(dict(long=p[0] or '', short=p[1] or '', desc=p[2] or '',
                        takes_raw=p[3], takes=(p[3] == 'true'),
                        alt=p[4], fields=f))
    return out

def label(r):
    return r['long'] or r['short'] or '<unnamed row>'

def check_table(table):
    seen = {}
    error_count = 0
    for r in table:
        if not r['long'] and not r['short']:
            print("Table row with no name")
            error_count += 1
        if len(r['short']) > 1:
            print(f"{label(r)}: short name '{r['short']}' is more than one character")
            error_count += 1
        if len(r['long']) == 1:
            print(f"{label(r)}: single-character long name, should it be the short name?")
            error_count += 1
        if re.search(r'[\s-]', r['long']):
            print(f"{label(r)}: long name contains a space or dash")
            error_count += 1
        if r['takes_raw'] not in (None, 'true', 'false'):
            print(f"{label(r)}: column 4 should be true or false, found '{r['takes_raw']}'")
            error_count += 1
        if len(r['fields']) > 5:
            print(f"{label(r)}: more than 5 columns")
            error_count += 1
        # if not r['desc'].strip():
        #     print(f"{label(r)}: no help text")
        #     error_count += 1
        if r['alt'] and not r['takes']:
            print(f"{label(r)}: has a help name for its value but takes_a_value is false")
            error_count += 1
        if r['takes'] and not r['long'] and not r['alt']:
            print(f"{label(r)}: short-only option takes a value but has no help name")
            error_count += 1
        for n in (r['long'], r['short']):
            if n:
                if n in seen:
                    print(f"Duplicate option name in table: {n}")
                    error_count += 1
                seen[n] = r

    return error_count

def check_handler(table, src):
    by_form = {}
    error_count = 0
    for r in table:
        if r['long']:
            by_form['--' + r['long']] = r
        if r['short']:
            by_form['-' + r['short']] = r

    cond = re.compile(r'\bif\s*\(([^)]*\bopt\s*==[^)]*)\)')
    ms = list(cond.finditer(src))

    tested = {}
    for m in ms:
        for lit in re.findall(rf'\bopt\s*==\s*({STR})', m.group(1)):
            tested[lit] = tested.get(lit, 0) + 1
    for lit, n in tested.items():
        if n > 1:
            print(f"Handler tests {lit} {n} times, so a later branch can never run")
            error_count += 1

    handled = {}                                  # id(row) -> set of forms tested

    for k, m in enumerate(ms):
        end = ms[k + 1].start() if k + 1 < len(ms) else len(src)
        body = re.split(r'\belse\s*\{', src[m.end():end])[0]
        uses_val = bool(re.search(r'\bval\b', body))   # adjust if you rename it

        for lit in re.findall(rf'\bopt\s*==\s*({STR})', m.group(1)):
            form = lit[1:-1]
            r = by_form.get(form)
            if r is None:
                near = [f for f in by_form if f.lower() == form.lower()]
                hint = f" (table has '{near[0]}')" if near else ""
                print(f"Handler tests '{form}' which is not in the table{hint}")
                error_count += 1
                continue
            handled.setdefault(id(r), set()).add(form)
            if uses_val and not r['takes']:
                print(f"{form}: handler uses val but table says takes_a_value is false")
                error_count += 1
            if r['takes'] and not uses_val:
                print(f"{form}: table says it takes a value but the handler never uses val")
                error_count += 1

    for r in table:
        forms = handled.get(id(r))
        if not forms:
            print(f"{label(r)}: in the table but never tested in the handler")
            error_count += 1
            continue
        # canonical form: short name if there is one, else long name
        want = '-' + r['short'] if r['short'] else '--' + r['long']
        if want not in forms:
            print(f"{label(r)}: handler should test '{want}' but tests {sorted(forms)}")
            error_count += 1

    return error_count

if __name__ == "__main__":

    failed = []
    for path in sys.argv[1:]:
        # print(f"== {path}")
        src = open(path).read()
        table = parse_table(src)
        if len(table) == 0:
            failed.append(path)
            continue
        if check_table(table) != 0:
            failed.append(path)
            continue
        if check_handler(table, src) != 0:
            failed.append(path)
            continue

    if len(failed):
        # note that when run inside visual studio writing to stderr
        # does not get printed on the screen - hence just regular print
        print("Failures detected in %s" % ", ".join(failed))
        raise SystemExit(1)



