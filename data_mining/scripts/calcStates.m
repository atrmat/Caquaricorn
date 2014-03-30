clear all;
path='permute_';
fileType='.csv';
plotType=['.','h','o','x','*','s','d','v','^','<','>','p','+']; % 13 plot types
plotNum=length(plotType)-1;
colorType=['b','r','g','m','c','k','y']; % 8 color types,  ,'w',
colorNum=length(colorType)-1;
N=6;
for i=1:1:N
    fileNum = num2str(i);
    filePath=[path,fileNum,fileType];
    sgc_exist = exist(filePath, 'file'); 
    if sgc_exist ~= 2 %% if the file doesn't exist, continue loop
        continue;
    end
    result=importdata(filePath);
    r7=result.data(:,7);
    pt = mod(i,plotNum)+1;
    ct = mod(i,colorNum)+1;
    plot(r7,['-',colorType(ct),plotType(pt)]);
    axis([0 127 0 801365]);
    grid on;
    hold on;
    xlabel('states');
    ylabel('num');
end
legend('第一时段','第二时段','第三时段','第四时段','第五时段','第六时段');
saveas(gcf,[path,'all clusters','.png']);
% close all;
    