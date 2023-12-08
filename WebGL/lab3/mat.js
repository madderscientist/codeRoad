// 矩阵乘法，行主，4*4
function MultiMat4(a, b) {
    let a00 = a[0], a01 = a[1], a02 = a[2], a03 = a[3];
    let a10 = a[4], a11 = a[5], a12 = a[6], a13 = a[7];
    let a20 = a[8], a21 = a[9], a22 = a[10], a23 = a[11];
    let a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];
    let b00 = b[0], b01 = b[1], b02 = b[2], b03 = b[3];
    let b10 = b[4], b11 = b[5], b12 = b[6], b13 = b[7];
    let b20 = b[8], b21 = b[9], b22 = b[10], b23 = b[11];
    let b30 = b[12], b31 = b[13], b32 = b[14], b33 = b[15];
    return new Float32Array([
        a00 * b00 + a01 * b10 + a02 * b20 + a03 * b30,
        a00 * b01 + a01 * b11 + a02 * b21 + a03 * b31,
        a00 * b02 + a01 * b12 + a02 * b22 + a03 * b32,
        a00 * b03 + a01 * b13 + a02 * b23 + a03 * b33,
        a10 * b00 + a11 * b10 + a12 * b20 + a13 * b30,
        a10 * b01 + a11 * b11 + a12 * b21 + a13 * b31,
        a10 * b02 + a11 * b12 + a12 * b22 + a13 * b32,
        a10 * b03 + a11 * b13 + a12 * b23 + a13 * b33,
        a20 * b00 + a21 * b10 + a22 * b20 + a23 * b30,
        a20 * b01 + a21 * b11 + a22 * b21 + a23 * b31,
        a20 * b02 + a21 * b12 + a22 * b22 + a23 * b32,
        a20 * b03 + a21 * b13 + a22 * b23 + a23 * b33,
        a30 * b00 + a31 * b10 + a32 * b20 + a33 * b30,
        a30 * b01 + a31 * b11 + a32 * b21 + a33 * b31,
        a30 * b02 + a31 * b12 + a32 * b22 + a33 * b32,
        a30 * b03 + a31 * b13 + a32 * b23 + a33 * b33
    ]);
}
// 叉乘
function Cross(a, b) {
    return new Float32Array([
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    ]);
}
// 模
function absVec3(x) {
    return Math.sqrt(x[0]**2+x[1]**2+x[2]**2);
}
// 归一化
function normalVec3(x) {
    let nx = absVec3(x);
    return new Float32Array([x[0]/nx, x[1]/nx, x[2]/nx]);
}
function MultiVec4(M, V) {
    return [
        M[0] * V[0] + M[1] * V[1] + M[2] * V[2] + M[3] * V[3],
        M[4] * V[0] + M[5] * V[1] + M[6] * V[2] + M[7] * V[3],
        M[8] * V[0] + M[9] * V[1] + M[10] * V[2] + M[11] * V[3],
        M[12] * V[0] + M[13] * V[1] + M[14] * V[2] + M[15] * V[3]
    ];
}
function inverseMat4(M) {
    let m00 = M[0], m01 = M[1], m02 = M[2], m03 = M[3];
    let m10 = M[4], m11 = M[5], m12 = M[6], m13 = M[7];
    let m20 = M[8], m21 = M[9], m22 = M[10], m23 = M[11];
    let m30 = M[12], m31 = M[13], m32 = M[14], m33 = M[15];
    let det00 = m11 * m22 * m33 + m12 * m23 * m31 + m13 * m21 * m32 - m13 * m22 * m31 - m12 * m21 * m33 - m11 * m23 * m32;
    let det01 = m10 * m22 * m33 + m12 * m23 * m30 + m13 * m20 * m32 - m13 * m22 * m30 - m12 * m20 * m33 - m10 * m23 * m32;
    let det02 = m10 * m21 * m33 + m11 * m23 * m30 + m13 * m20 * m31 - m13 * m21 * m30 - m11 * m20 * m33 - m10 * m23 * m31;
    let det03 = m10 * m21 * m32 + m11 * m22 * m30 + m12 * m20 * m31 - m12 * m21 * m30 - m11 * m20 * m32 - m10 * m22 * m31;
    let det = m00 * det00 - m01 * det01 + m02 * det02 - m03 * det03;
    if (det === 0) return null;
    let invDet = 1 / det;
    return new Float32Array([
        det00 * invDet,
        -(m01 * m22 * m33 + m02 * m23 * m31 + m03 * m21 * m32 - m03 * m22 * m31 - m02 * m21 * m33 - m01 * m23 * m32) * invDet,
        (m01 * m12 * m33 + m02 * m13 * m31 + m03 * m11 * m32 - m03 * m12 * m31 - m02 * m11 * m33 - m01 * m13 * m32) * invDet,
        -(m01 * m12 * m23 + m02 * m13 * m21 + m03 * m11 * m22 - m03 * m12 * m21 - m02 * m11 * m23 - m01 * m13 * m22) * invDet,
        -det01 * invDet,
        (m00 * m22 * m33 + m02 * m23 * m30 + m03 * m20 * m32 - m03 * m22 * m30 - m02 * m20 * m33 - m00 * m23 * m32) * invDet,
        -(m00 * m12 * m33 + m02 * m13 * m30 + m03 * m10 * m32 - m03 * m12 * m30 - m02 * m10 * m33 - m00 * m13 * m32) * invDet,
        (m00 * m12 * m23 + m02 * m13 * m20 + m03 * m10 * m22 - m03 * m12 * m20 - m02 * m10 * m23 - m00 * m13 * m22) * invDet,
        det02 * invDet,
        -(m00 * m21 * m33 + m01 * m23 * m30 + m03 * m20 * m31 - m03 * m21 * m30 - m01 * m20 * m33 - m00 * m23 * m31) * invDet,
        (m00 * m11 * m33 + m01 * m13 * m30 + m03 * m10 * m31 - m03 * m11 * m30 - m01 * m10 * m33 - m00 * m13 * m31) * invDet,
        -(m00 * m11 * m23 + m01 * m13 * m20 + m03 * m10 * m21 - m03 * m11 * m20 - m01 * m10 * m23 - m00 * m13 * m21) * invDet,
        -det03 * invDet,
        (m00 * m21 * m32 + m01 * m22 * m30 + m02 * m20 * m31 - m02 * m21 * m30 - m01 * m20 * m32 - m00 * m22 * m31) * invDet,
        -(m00 * m11 * m32 + m01 * m12 * m30 + m02 * m10 * m31 - m02 * m11 * m30 - m01 * m10 * m32 - m00 * m12 * m31) * invDet,
        (m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m01 * m10 * m22 - m00 * m12 * m21) * invDet
    ]);
}
function transposeMat4(M) {
    return new Float32Array([
        M[0], M[4], M[8], M[12],
        M[1], M[5], M[9], M[13],
        M[2], M[6], M[10], M[14],
        M[3], M[7], M[11], M[15]
    ]);
}