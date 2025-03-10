function cqtKernel = genCQTkernel(fmax, bins, fs, varargin)
%Calculating the CQT Kernel for one octave. All atoms are center-stacked.
%Atoms are placed so that the stacks of lower octaves are centered at the
%same positions in time, however, their amount is reduced by factor two for
%each octave down.
%
%INPUT:
%   fmax          ... highest frequency of interest
%   bins          ... number of bins per octave
%   fs            ... sampling frequency
%
%optional input parameters (parameter name/value pairs):
%
%   'q'             ... Q scaling factor. Default: 1.
%   'atomHopFactor' ... relative hop size corresponding to the shortest
%                       temporal atom. Default: 0.25.
%   'thresh'        ... values smaller than 'tresh' in the spectral kernel are rounded to
%                       zero. Default: 0.0005.
%   'win'           ... defines which window will be used for the CQT. Valid
%                       values are: 'blackman','hann' and 'blackmanharris'. To
%                       use the square root of eac h window use the prefix 'sqrt_'
%                      (i.e. 'sqrt_blackman'). Default: 'sqrt_blackmanharris'
%   'perfRast'      ... if set to 1 the kernel is designed in order to
%                       enable perfect rasterization using the function
%                       cqtPerfectRast() (Default: perRast=0). See documentation of
%                       'cqtPerfectRast' for further information.
%
%OUTPUT:
%   cqtKernel   ... Structure that contains the spectral kernel 'fKernel'
%                   additional design parameters used in cqt(), cqtPerfectRast() and icqt().
%
%Christian Sch�rkhuber, Anssi Klapuri 2010-06

%% input parameters
q = 1; %default value
atomHopFactor = 0.25; %default value
thresh = 0.0005; %default value
winFlag = 'sqrt_blackmanharris'; %default value
perfRast = 0; %default value

for ain = 1:length(varargin)
    if strcmp(varargin{ain},'q'), q = varargin{ain+1}; end;
    if strcmp(varargin{ain},'atomHopFactor'), atomHopFactor = varargin{ain+1}; end;
    if strcmp(varargin{ain},'thresh'), thresh = varargin{ain+1}; end;
    if strcmp(varargin{ain},'win'), winFlag = varargin{ain+1}; end;
    if strcmp(varargin{ain},'perfRast'), perfRast = varargin{ain+1}; end;
end

%% define
fmin = (fmax/2)*2^(1/bins);   % 同cqt.m: 除2是上一个八度的最后一个音，乘2^(1/bins)变成本八度第一个音
Q = 1/(2^(1/bins)-1);    % 本音频率是f，下一个音频率是f*2^(1/bins)，因此滤波器带宽为B=(2^(1/bins)-1)f，则Q = f/B = 1/(2^(1/bins)-1)。但是我认为应该是1/(2^(1/24)-2^(-1/24))
Q = Q*q;
Nk_max = Q * fs / fmin; 
Nk_max = round(Nk_max); %length of the largest atom [samples] fs/N = f/Q，即△f=B。频率越小要的点数越多，所以f_min对N_max


%% Compute FFT size, FFT hop, atom hop, ...
Nk_min = round( Q * fs / (fmin*2^((bins-1)/bins)) ); %length of the shortest atom [samples] 把f_min的定义带入其实就是f_max
atomHOP = round(Nk_min*atomHopFactor); %atom hop size 每个频点的窗的时移点数 用最短的窗算
first_center = ceil(Nk_max/2); %first possible center position within the frame 最低频率的窗的中心点
first_center = atomHOP * ceil(first_center/atomHOP); %lock the first center to an integer multiple of the atom hop size 为了频点的对齐，把时移的最小单位设置为atomHOP。用ceil是为了不截断窗
FFTLen = 2^nextpow2(first_center+ceil(Nk_max/2)); %use smallest possible FFT size (increase sparsity) 一个FFT帧内有好几个时移窗，选2的幂（便于FFT）把最长的窗装下，多余的0可以放置时移窗

% 看看可以放几个（间隔为atomHOP，长度为Nk_max的）
if perfRast
    winNr = floor((FFTLen-ceil(Nk_max/2)-first_center)/atomHOP); %number of temporal atoms per FFT Frame
    if winNr == 0
        FFTLen = FFTLen * 2;
        winNr = floor((FFTLen-ceil(Nk_max/2)-first_center)/atomHOP);
    end
else % 0个间隔就是平移0次就是1个窗
    winNr = floor((FFTLen-ceil(Nk_max/2)-first_center)/atomHOP)+1; %number of temporal atoms per FFT Frame
end

last_center = first_center + (winNr-1)*atomHOP;
fftHOP = (last_center + atomHOP) - first_center; % hop size of FFT frames  保证下一帧的第一个窗和本帧的最后一个窗距离也是atomHOP
fftOLP = (FFTLen-fftHOP/FFTLen)*100; %overlap of FFT frames in percent ***AK:needed?

%% init variables
tempKernel= zeros(1,FFTLen); 
sparKernel= []; 

%% Compute kernel
atomInd = 0;
for k = 1:bins
   fk = fmin*2^((k-1)/bins);
   Nk = round( Q * fs / fk ); %N[k] = (fs/fk)*Q. Rounding will be omitted in future versions
   
   switch winFlag
       case 'sqrt_blackmanharris'
            winFct = sqrt(blackmanharris(Nk));
       case 'blackmanharris'
            winFct = blackmanharris(Nk);
       case 'sqrt_hann'
            winFct = sqrt(hann(Nk,'periodic'));
       case 'hann'
            winFct = hann(Nk,'periodic');
       case 'sqrt_blackman'
            winFct = sqrt(hann(blackman,'periodic'));
       case 'blackman'
            winFct = blackman(Nk,'periodic');
       otherwise
            winFct = sqrt(blackmanharris(Nk));
            if k==1, warning('CQT:INPUT','Non-existing window function. Default window is used!'); end;
   end
   
   tempKernelBin = (winFct/Nk) .* exp(2*pi*1i*fk*(0:Nk-1)'/fs);
   atomOffset = first_center - ceil(Nk/2);

   for i = 1:winNr 
       shift = atomOffset + ((i-1) * atomHOP);
       tempKernel(1+shift:Nk+shift) = tempKernelBin; 
       atomInd = atomInd+1;
       specKernel= fft(tempKernel);
       specKernel(abs(specKernel)<=thresh)= 0;   
       sparKernel= sparse([sparKernel; specKernel]);
       tempKernel = zeros(1,FFTLen); %reset window     
   end
end
sparKernel = (sparKernel.')/FFTLen;

%% Normalize the magnitudes of the atoms
[ignore,wx1]=max(sparKernel(:,1));
[ignore,wx2]=max(sparKernel(:,end));
wK=sparKernel(wx1:wx2,:);
wK = diag(wK * wK');
wK = wK(round(1/q)+1:(end-round(1/q)-2));
weight = 1./mean(abs(wK));
weight = weight.*(fftHOP/FFTLen); 
weight = sqrt(weight); %sqrt because the same weight is applied in icqt again
sparKernel = weight.*sparKernel;

%% return
cqtKernel = struct('fKernel',sparKernel,'fftLEN',FFTLen,'fftHOP',fftHOP,'fftOverlap',fftOLP,'perfRast',perfRast,...
    'bins',bins,'firstcenter',first_center,'atomHOP',atomHOP,'atomNr',winNr,'Nk_max',Nk_max,'Q',Q,'fmin',fmin);
