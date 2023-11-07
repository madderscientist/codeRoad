function createHorse() {
    let body = new GLobj(
        GLroot.gl,
        GLobj.translation(0),
        GLobj.rotation(0),
        GLmesh_E.cubetest(),
        GLobj.scale(n(380), n(165), n(165))
    );
    // 尾巴
    let tail = new GLobj(
        GLroot.gl,
        GLobj.translation(n(-200), 0, 0),
        GLobj.rotation(0, 0, -Math.PI / 6),
        GLmesh_E.cube([0.4, 1, 0]),
        GLobj.scale(n(50), n(150), n(40))
    );
    body.addChild('tail', tail);
    // JJ
    let JJ = new GLobj(
        GLroot.gl,
        GLobj.translation(n(-140), n(-76), 0),
        GLobj.rotation(0, 0, Math.PI / 3),
        GLmesh_E.cube([0, 0, 0]),
        GLobj.scale(n(20), n(100), n(20))
    );
    body.addChild('JJ', JJ);
    // 腿
    let leg1 = new GLobj(
        GLroot.gl,
        GLobj.translation(n(-157.4), n(-132.4), n(49.9)),
        GLobj.I,
        GLmesh_E.cube([0, 1, 1]),
        GLobj.scale(n(65), n(230), n(65))
    );
    body.addChild('leg1', leg1);
    let leg2 = new GLobj(
        GLroot.gl,
        GLobj.translation(n(-157.4), n(-132.4), n(-49.9)),
        GLobj.I,
        GLmesh_E.cube([0, 1, 1]),
        GLobj.scale(n(65), n(230), n(65))
    );
    body.addChild('leg2', leg2);
    let leg3 = new GLobj(
        GLroot.gl,
        GLobj.translation(n(157.4), n(-132.4), n(49.9)),
        GLobj.I,
        GLmesh_E.cube([0, 1, 1]),
        GLobj.scale(n(65), n(230), n(65))
    );
    body.addChild('leg3', leg3);
    let leg4 = new GLobj(
        GLroot.gl,
        GLobj.translation(n(157.4), n(-132.4), n(-49.9)),
        GLobj.I,
        GLmesh_E.cube([0, 1, 1]),
        GLobj.scale(n(65), n(230), n(65))
    );
    body.addChild('leg4', leg4);

    let neck = new GLobj(
        GLroot.gl,
        GLobj.translation(n(200), n(125), 0),
        GLobj.rotation(0, 0, -Math.PI / 6),
        GLmesh_E.cube([0, 1, 1]),
        GLobj.scale(n(120), n(250), n(65))
    );
    body.addChild('neck', neck);
    let bristle = new GLobj(
        GLroot.gl,
        GLobj.translation(n(-75), 0, 0),
        GLobj.rotation(0),
        GLmesh_E.cubetest(),
        GLobj.scale(n(30), n(250), n(10))
    );
    neck.addChild('bristle', bristle);

    let headSet = new GLobj(   // 一个不包含mesh的集合
        GLroot.gl,
        GLobj.translation(n(-60), n(125), 0),   // 移动原点
        GLobj.rotation(0),
        null
    );
    neck.addChild('headSet', headSet);
    let head = new GLobj(
        GLroot.gl,
        GLobj.translation(n(57), n(-42.4), 0),
        GLobj.rotation(0),
        GLmesh_E.cubetest(),
        GLobj.scale(n(115), n(85), n(75))
    );
    headSet.addChild('head', head);
    let mouse = new GLobj(
        GLroot.gl,
        GLobj.translation(n(155), n(-40), 0),
        GLobj.rotation(0),
        GLmesh_E.cubetest(),
        GLobj.scale(n(80), n(80), n(70))
    );
    headSet.addChild('mouse', mouse);
    let ear_l = new GLobj(
        GLroot.gl,
        GLobj.translation(n(12.5), n(15), n(25)),
        GLobj.rotation(0),
        GLmesh_E.cubetest(),
        GLobj.scale(n(25), n(30), n(15))
    );
    headSet.addChild('ear_l', ear_l);
    let ear_r = new GLobj(
        GLroot.gl,
        GLobj.translation(n(12.5), n(15), n(-25)),
        GLobj.rotation(0),
        GLmesh_E.cubetest(),
        GLobj.scale(n(25), n(30), n(15))
    );
    headSet.addChild('ear_r', ear_r);

    body.stand = function () {  // 必须用function而不是()=>{}，因为this指向
        // this.transform = GLobj.translation(this.transform[3], this.transform[7], this.transform[11]);
        this.children['leg1'].transform = GLobj.I;
        this.children['leg2'].transform = GLobj.I;
        this.children['leg3'].transform = GLobj.I;
        this.children['leg4'].transform = GLobj.I;
        this.children['neck'].transform = GLobj.I;
        this.children['tail'].transform = GLobj.I;
    }
    body.run1 = function () {
        let [tx, itx] = [GLobj.translation(0, n(115), 0), GLobj.translation(0, n(-115), 0)];
        let [ro1, ro2] = [MultiMat4(tx, MultiMat4(GLobj.rotation(0, 0, -Math.PI / 4), itx)), MultiMat4(tx, MultiMat4(GLobj.rotation(0, 0, Math.PI / 4), itx))]
        body.children['leg1'].transform = ro1;
        body.children['leg2'].transform = ro2;
        body.children['leg3'].transform = ro2;
        body.children['leg4'].transform = ro1;
    }
    body.run2 = function () {
        let [tx, itx] = [GLobj.translation(0, n(115), 0), GLobj.translation(0, n(-115), 0)];
        let [ro1, ro2] = [MultiMat4(tx, MultiMat4(GLobj.rotation(0, 0, -Math.PI / 4), itx)), MultiMat4(tx, MultiMat4(GLobj.rotation(0, 0, Math.PI / 4), itx))]
        body.children['leg1'].transform = ro2;
        body.children['leg2'].transform = ro1;
        body.children['leg3'].transform = ro1;
        body.children['leg4'].transform = ro2;
    }
    body.rest = function () { // 伸懒腰
        body.transformation(GLobj.rotation(0, 0, -Math.PI / 15));
        let [tx, itx] = [GLobj.translation(0, n(115), 0), GLobj.translation(0, n(-115), 0)];
        let legt = MultiMat4(tx, MultiMat4(GLobj.rotation(0, 0, Math.PI / 7), itx))
        body.children['leg1'].transform = legt;
        body.children['leg2'].transform = legt;
        body.children['leg3'].transform = MultiMat4(tx, MultiMat4(GLobj.rotation(0, 0, Math.PI / 3), itx));
        body.children['leg4'].transform = MultiMat4(tx, MultiMat4(GLobj.rotation(0, 0, Math.PI / 3), itx));
        body.children['neck'].transform = MultiMat4(GLobj.translation(0, n(-125), 0), MultiMat4(GLobj.rotation(0, 0, Math.PI / 4), GLobj.translation(0, n(125), 0)));
        body.children['tail'].transform = MultiMat4(GLobj.translation(n(-25), n(75), 0), MultiMat4(GLobj.rotation(0, 0, -Math.PI / 6), GLobj.translation(n(25), n(-75), 0)));
    }
    return body;
}